//
//  AppDelegate.h
//  Plasticity Viewer
//
//  Created by Seo Townsend on 6/29/13.
//  Copyright (c) 2013 Layer7 Enterprise. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DebugManager.h"

@interface AppDelegate : NSObject <NSApplicationDelegate, DebugManagerDelegate, NSMenuDelegate>

@property (assign) IBOutlet NSWindow *window;
@property DebugManager *debugManager;

@property (assign) IBOutlet NSTextField *plasticity;
@property (assign) IBOutlet NSPopUpButton *selectNeuron;

@end
