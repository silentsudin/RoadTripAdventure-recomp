#include "platform/Dialogs.h"

#import <AppKit/AppKit.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

#include <iostream>

namespace rt::dialogs
{
    namespace
    {
        void ensureApp()
        {
            [NSApplication sharedApplication];
            [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
            [NSApp activateIgnoringOtherApps:YES];
        }
    }

    std::optional<std::filesystem::path> pickRomImage()
    {
        @autoreleasepool
        {
            ensureApp();
            NSOpenPanel *panel = [NSOpenPanel openPanel];
            panel.title = @"Locate your Road Trip (USA) disc image";
            panel.message = @"Choose the .cue, .bin, .iso or .chd you made from your own copy of Road Trip (SLUS-20398).";
            panel.canChooseFiles = YES;
            panel.canChooseDirectories = NO;
            panel.allowsMultipleSelection = NO;
            NSMutableArray<UTType *> *types = [NSMutableArray array];
            for (NSString *ext in @[ @"cue", @"bin", @"iso", @"chd" ])
            {
                if (UTType *t = [UTType typeWithFilenameExtension:ext])
                    [types addObject:t];
            }
            panel.allowedContentTypes = types;
            if ([panel runModal] != NSModalResponseOK || panel.URLs.count == 0)
                return std::nullopt;
            return std::filesystem::path(panel.URLs.firstObject.fileSystemRepresentation);
        }
    }

    bool message(const std::string &title, const std::string &text, bool withCancel)
    {
        @autoreleasepool
        {
            ensureApp();
            NSAlert *alert = [[NSAlert alloc] init];
            alert.messageText = [NSString stringWithUTF8String:title.c_str()];
            alert.informativeText = [NSString stringWithUTF8String:text.c_str()];
            [alert addButtonWithTitle:@"OK"];
            if (withCancel)
                [alert addButtonWithTitle:@"Cancel"];
            return [alert runModal] == NSAlertFirstButtonReturn;
        }
    }
}
