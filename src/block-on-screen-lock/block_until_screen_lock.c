#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>
#include <stdlib.h>     // For system()
#include <string.h>     // For snprintf()
#include <sys/wait.h>   // For WIFEXITED, WEXITSTATUS
#include <stdbool.h>     // For bool type (requires C99 or later)
#include <IOKit/IOKitLib.h>

// Callback function for when the screen is locked
void exitCallback(CFNotificationCenterRef center,
                          void *observer,
                          CFStringRef name,
                          const void *object,
                          CFDictionaryRef userInfo) {
    exit(0);
}

// Function to check if the screen is currently locked.
// Returns true if the screen is locked, false otherwise.
// "false" is also returned if the state cannot be determined (e.g., key missing, error).
bool getScreenIsLockedState() {
    bool isLocked = false; // Default to not locked
    io_registry_entry_t rootEntry = MACH_PORT_NULL;
    CFMutableDictionaryRef properties = NULL;

    // Get the IORegistry root entry. This is the starting point, similar to `ioreg -n Root`.
    rootEntry = IORegistryGetRootEntry(kIOMainPortDefault);
    if (rootEntry == MACH_PORT_NULL) {
        // fprintf(stderr, "Error: Could not get IORegistry root entry.\n");
        return false; // Cannot determine state, assume not locked for safety
    }

    // Get properties of the root entry. The key :IOConsoleUsers:0:CGSSessionScreenIsLocked
    // is typically found within the properties of the root registry object.
    kern_return_t kr = IORegistryEntryCreateCFProperties(rootEntry, &properties, kCFAllocatorDefault, kNilOptions);

    // Release the IORegistry entry object as it's no longer needed after this point.
    IOObjectRelease(rootEntry);
    rootEntry = MACH_PORT_NULL; //ป้องกันการใช้งานซ้ำ

    if (kr != KERN_SUCCESS || properties == NULL) {
        // fprintf(stderr, "Error: Could not get properties for IORegistry root entry. kr = %d\n", kr);
        if (properties != NULL) {
            CFRelease(properties);
        }
        return false; // Cannot determine state
    }

    // Navigate the dictionary structure to find the CGSSessionScreenIsLocked key.
    // Path: Root -> "IOConsoleUsers" (Array) -> Index 0 (Dictionary) -> "CGSSessionScreenIsLocked" (Boolean)

    CFTypeRef consoleUsersObj = CFDictionaryGetValue(properties, CFSTR("IOConsoleUsers"));
    if (consoleUsersObj != NULL && CFGetTypeID(consoleUsersObj) == CFArrayGetTypeID()) {
        CFArrayRef consoleUsersArray = (CFArrayRef)consoleUsersObj;

        if (CFArrayGetCount(consoleUsersArray) > 0) {
            // Get the first user's dictionary (index 0)
            CFTypeRef firstUserObj = CFArrayGetValueAtIndex(consoleUsersArray, 0);
            if (firstUserObj != NULL && CFGetTypeID(firstUserObj) == CFDictionaryGetTypeID()) {
                CFDictionaryRef firstUserDict = (CFDictionaryRef)firstUserObj;

                // Get the screen lock status boolean
                CFTypeRef screenIsLockedObj = CFDictionaryGetValue(firstUserDict, CFSTR("CGSSessionScreenIsLocked"));
                if (screenIsLockedObj != NULL && CFGetTypeID(screenIsLockedObj) == CFBooleanGetTypeID()) {
                    isLocked = CFBooleanGetValue((CFBooleanRef)screenIsLockedObj);
                }
            }
        }
    }

    // Release the properties dictionary
    CFRelease(properties);

    return isLocked;
}

void usage() {
  printf("USAGE\n   dk block-until-screen-lock <locked|unlocked>\n");
  exit(0);
}
int main(int argc, const char * argv[]) {
    if (argc == 1 || strcmp("--help", argv[1]) == 0) {
      usage();
    }
    if (strcmp("--summary", argv[1]) == 0) {
      printf("Block until screen is locked or unlocked");
      exit(0);
    }
    // True means we are waiting for the screen to be locked. False means we
    // are waiting for an unlock.
    bool waitForLocked;
    if (strcmp("locked", argv[1]) == 0) {
      waitForLocked = 1;
    } else if (strcmp("unlocked", argv[1]) == 0) {
      waitForLocked = 0;
    } else {
      usage();
    }

    // Check for current state, in case we locked or unlocked before this
    // program was started.
    // Note that checking before starting the notifications observer can still
    // result in races where the lock state changes between this check and the
    // notification observer starting, but the window is small so we are
    // ignoring that for simplicity.
    bool currentLockState = getScreenIsLockedState();
    if (waitForLocked && currentLockState) {
      exit(0);
    }
    if (!waitForLocked && !currentLockState) {
      exit(0);
    }

    // Get a reference to the distributed notification center
    CFNotificationCenterRef distributedCenter = CFNotificationCenterGetDistributedCenter();
    if (!distributedCenter) {
        fprintf(stderr, "Error: Failed to get the distributed notification center.\n");
        return 1;
    }

    // Add an observer for the screen lock notification
    if (waitForLocked) {
      CFNotificationCenterAddObserver(distributedCenter,
          NULL, // The observer (NULL for a C function callback)
          exitCallback, // The callback function
          CFSTR("com.apple.screenIsLocked"), // Notification name
          NULL, // Object to observe (NULL for any object)
          CFNotificationSuspensionBehaviorDeliverImmediately);
    } else {
      // Add an observer for the screen unlock notification
      CFNotificationCenterAddObserver(distributedCenter,
          NULL,
          exitCallback,
          CFSTR("com.apple.screenIsUnlocked"),
          NULL,
          CFNotificationSuspensionBehaviorDeliverImmediately);
    }

    // Start the main run loop to receive notifications.
    // This function will not return until the run loop is stopped (e.g., by CFRunLoopStop)
    // or the program is terminated.
    CFRunLoopRun();
    return 0;
}
