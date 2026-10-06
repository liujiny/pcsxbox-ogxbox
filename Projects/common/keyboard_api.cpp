#define DEBUG_KEYBOARD
#include <xtl.h>
#include "keyboard_api.h"

extern "C" void sprintfx( const char *fmt, ... );

static HANDLE g_hKeyboardDevice[4] = { 0 };

static BOOL                     g_bKeyboardInitialised  = FALSE;
XINPUT_DEBUG_KEYSTROKE   g_keyboardStroke;

// initlializes the keyboard, or returns a 0 if unable to init, or a 1 if was able to init and found a keyboard
char Keyboard_Init( int queuesize, int delay, int interval )
{
	int t=0;

    // Check that we are not already initialized and then initialize if necessary
    if( g_bKeyboardInitialised )
        return 1;

    XINPUT_DEBUG_KEYQUEUE_PARAMETERS keyboardSettings;
    //keyboardSettings.dwFlags          = XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN|XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT;
    keyboardSettings.dwFlags          = XINPUT_DEBUG_KEYQUEUE_FLAG_KEYDOWN|XINPUT_DEBUG_KEYQUEUE_FLAG_KEYREPEAT|XINPUT_DEBUG_KEYQUEUE_FLAG_KEYUP|XINPUT_DEBUG_KEYQUEUE_FLAG_ONE_QUEUE;
    keyboardSettings.dwQueueSize      = queuesize;
    keyboardSettings.dwRepeatDelay    = delay;
    keyboardSettings.dwRepeatInterval = interval;

    if( ERROR_SUCCESS != XInputDebugInitKeyboardQueue( &keyboardSettings ) )
        return 0;  // couldnt initialize

    g_bKeyboardInitialised = TRUE;

    // Now find the keyboard device, in this case we shall loop indefinitely, although
    // it would be better to monitor the time taken and to time out if necessary
    // in case the keyboard has been unplugged

    DWORD dwDeviceMask = XGetDevices( XDEVICE_TYPE_DEBUG_KEYBOARD );

	sprintfx( "device mask %X\r\n", dwDeviceMask ) ;

    // Open the devices
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
		sprintfx( "checking port %u\r\n", i ) ;

        if( dwDeviceMask & (1<<i) ) 
        {
			sprintfx( "found port %u\r\n", i ) ;
			t++;
            // Now open the device
            XINPUT_POLLING_PARAMETERS pollValues;
            pollValues.fAutoPoll       = TRUE;
            pollValues.fInterruptOut   = TRUE;
            pollValues.bInputInterval  = 32;  
            pollValues.bOutputInterval = 32;
            pollValues.ReservedMBZ1    = 0;
            pollValues.ReservedMBZ2    = 0;

            g_hKeyboardDevice[i] = XInputOpen( XDEVICE_TYPE_DEBUG_KEYBOARD, i, XDEVICE_NO_SLOT, &pollValues );

			if ( g_hKeyboardDevice[i] == NULL )
			{
				sprintfx( "keyboard couldn't be opened %u\r\n", GetLastError() ) ;
			}
        }
    }
	
    return 1;
}




// checks if keyboards has been connected, and tells you how many is connected (!!)   could be useful at least.
char Keyboard_Status( )
{
	int i,t=0;
	DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_DEBUG_KEYBOARD, &dwInsertions, &dwRemovals );

    // Loop through all ports
    for( i=0; i < XGetPortCount(); i++ )
    {
        // Handle removed devices.
        if( dwRemovals & (1<<i) )
        {
			//t++;
            XInputClose( g_hKeyboardDevice[i] );
            g_hKeyboardDevice[i] = NULL;
		}

		if ( g_hKeyboardDevice[i] )
			t++ ;
	}
	return t; // return number of keyboard found 
}


// get inputs, will return windows keycodes from the first keyboard found that gives a keypress
char Keyboard_GetVKInput()
{
	int i;

	DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_DEBUG_KEYBOARD, &dwInsertions, &dwRemovals );

    // Loop through all ports
    for( i=0; i < XGetPortCount(); i++ )
    {
        // Handle removed devices.
        if( dwRemovals & (1<<i) )
        {
            XInputClose( g_hKeyboardDevice[i] );
            g_hKeyboardDevice[i] = NULL;
		}
	}

	// go through the list of ports
    for( i=0; i < XGetPortCount(); i++ )
	{
        // Handle inserted devices
        if( dwInsertions & (1<<i) )
        {
            // Now open the device
            XINPUT_POLLING_PARAMETERS pollValues;
            pollValues.fAutoPoll       = TRUE;
            pollValues.fInterruptOut   = TRUE;
            pollValues.bInputInterval  = 32;  
            pollValues.bOutputInterval = 32;
            pollValues.ReservedMBZ1    = 0;
            pollValues.ReservedMBZ2    = 0;

            // TCR 1-14 Device Types
            g_hKeyboardDevice[i] = XInputOpen( XDEVICE_TYPE_DEBUG_KEYBOARD, i, XDEVICE_NO_SLOT, &pollValues );
        }

        // If we have a valid device, poll it's state and track button changes
        if( g_hKeyboardDevice[i] )
        {
            if( ERROR_SUCCESS == XInputDebugGetKeystroke( &g_keyboardStroke ) )
                return g_keyboardStroke.VirtualKey;
        }
    }

    return '\0';
}



// get input, will return keypress as ascii
char Keyboard_GetASCIIInput()
{
    // Get status about gamepad insertions and removals. Note that, in order to
    // not miss devices, we will check for removed device BEFORE checking for
    // insertions
    DWORD dwInsertions, dwRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_DEBUG_KEYBOARD, &dwInsertions, &dwRemovals );

    // Loop through all ports
    for( DWORD i=0; i < XGetPortCount(); i++ )
    {
        // Handle removed devices.
        if( dwRemovals & (1<<i) )
        {
            XInputClose( g_hKeyboardDevice[i] );
            g_hKeyboardDevice[i] = NULL;
        }

        // Handle inserted devices
        if( dwInsertions & (1<<i) )
        {
            // Now open the device
            XINPUT_POLLING_PARAMETERS pollValues;
            pollValues.fAutoPoll       = TRUE;
            pollValues.fInterruptOut   = TRUE;
            pollValues.bInputInterval  = 32;  
            pollValues.bOutputInterval = 32;
            pollValues.ReservedMBZ1    = 0;
            pollValues.ReservedMBZ2    = 0;

            // TCR 1-14 Device Types
            g_hKeyboardDevice[i] = XInputOpen( XDEVICE_TYPE_DEBUG_KEYBOARD, i, 
                                               XDEVICE_NO_SLOT, &pollValues );
        }

        // If we have a valid device, poll it's state and track button changes
        if( g_hKeyboardDevice[i] )
        {
            if( ERROR_SUCCESS == XInputDebugGetKeystroke( &g_keyboardStroke ) )
                return (g_keyboardStroke.Ascii);
        }
    }

    return '\0';
}




