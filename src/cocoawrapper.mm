#include "cocoawrapper.h"
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

void RequestUserAttention(void){
	[NSApp requestUserAttention:NSInformationalRequest];
}

const char * GetAppSupportDirectory(void){
	NSArray * dir = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	NSString * path = nil;
	if(dir){
		path = [dir objectAtIndex:0];
	}
	return [path fileSystemRepresentation];
}