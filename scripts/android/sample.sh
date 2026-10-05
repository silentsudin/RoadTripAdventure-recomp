#!/system/bin/sh
# sample.sh <pid> <seconds>: every ~0.2 s: uptime; per thread its stat and schedstat; core clocks; GPU busy % and clock.
PID=$1; END=$(( $(cut -d. -f1 /proc/uptime) + $2 ))
while :; do
  read -r up idle < /proc/uptime
  [ ${up%.*} -ge $END ] && break
  echo "T $up"
  for t in /proc/$PID/task/*; do
    read -r st < $t/stat; read -r sc < $t/schedstat
    echo "R $st | $sc"
  done
  f=""; for c in 0 1 2 3 4 5 6 7; do read -r v < /sys/devices/system/cpu/cpu$c/cpufreq/scaling_cur_freq; f="$f $v"; done
  echo "F$f"
  read -r gb < /sys/class/kgsl/kgsl-3d0/gpu_busy_percentage; read -r gc < /sys/class/kgsl/kgsl-3d0/gpuclk
  echo "G ${gb% *} 0 $gc"
  sleep 0.2
done
