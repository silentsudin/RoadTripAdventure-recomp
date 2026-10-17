#!/usr/bin/env python3
# Tiny localhost HTTP CONNECT proxy with DIY DNS (UDP to 8.8.8.8), for when
# the macOS resolver (mDNSResponder/getaddrinfo) is unresponsive but raw IP
# connectivity works. TLS tunnels byte-for-byte, so git/curl certificate
# verification against the real hostname stays fully intact.
import socket, struct, threading, selectors, sys, random

def dns_a(host):
    tid = random.randint(0, 0xFFFF)
    q = struct.pack('>HHHHHH', tid, 0x0100, 1, 0, 0, 0)
    for part in host.split('.'):
        q += bytes([len(part)]) + part.encode()
    q += b'\x00' + struct.pack('>HH', 1, 1)
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(5)
    s.sendto(q, ('8.8.8.8', 53))
    data, _ = s.recvfrom(4096)
    if len(data) < 12:
        return None
    _, _flags, _qd, ancount = struct.unpack('>HHHH', data[0:8])
    i = 12
    while i < len(data) and data[i] != 0:
        i += data[i] + 1
    i += 5
    for _ in range(ancount):
        if i + 1 > len(data):
            return None
        if data[i] & 0xC0 == 0xC0:
            i += 2
        else:
            while i < len(data) and data[i] != 0:
                i += data[i] + 1
            i += 1
        if i + 10 > len(data):
            return None
        rtype, _rclass, _ttl, rdlen = struct.unpack('>HHIH', data[i:i+10])
        i += 10
        if rtype == 1 and rdlen == 4 and i + 4 <= len(data):
            return socket.inet_ntoa(data[i:i+4])
        i += rdlen
    return None

def pump(a, b):
    sel = selectors.DefaultSelector()
    sel.register(a, selectors.EVENT_READ)
    sel.register(b, selectors.EVENT_READ)
    try:
        while True:
            for key, _ in sel.select(60):
                src = key.fileobj
                dst = b if src is a else a
                data = src.recv(65536)
                if not data:
                    return
                dst.sendall(data)
    except OSError:
        pass
    finally:
        sel.close()

def handle(client):
    try:
        req = b''
        while b'\r\n\r\n' not in req:
            chunk = client.recv(4096)
            if not chunk:
                return
            req += chunk
        parts = req.split(b'\r\n')[0].decode(errors='replace').split()
        if len(parts) < 2 or parts[0] != 'CONNECT':
            client.sendall(b'HTTP/1.1 405 Not Allowed\r\n\r\n')
            return
        host, _, port = parts[1].rpartition(':')
        ip = dns_a(host) or dns_a('www.' + host)
        if not ip:
            client.sendall(b'HTTP/1.1 502 DNS Failed\r\n\r\n')
            return
        upstream = socket.create_connection((ip, int(port or 443)), timeout=10)
        client.sendall(b'HTTP/1.1 200 Connection Established\r\n\r\n')
        pump(client, upstream)
    except Exception as exc:
        print(f'conn error: {exc}', flush=True)
    finally:
        try:
            client.close()
        except OSError:
            pass

def main():
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 8899
    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(('127.0.0.1', port))
    srv.listen(32)
    print(f'dnsproxy listening on 127.0.0.1:{port}', flush=True)
    while True:
        client, _ = srv.accept()
        threading.Thread(target=handle, args=(client,), daemon=True).start()

if __name__ == '__main__':
    main()
