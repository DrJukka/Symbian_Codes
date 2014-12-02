Note that this application does not support back upping.


--------------- main appplication ------------------

This application is extended task manager which also gives you extended access to wide variety of device functionality.

All functionality is implemented as separate add-on plug-ins which can be added on later without re-installation of this main application.

To use different functionalities click the icon or select open command from options menu.

The plug-in list can be viewed as a list or as a list, the type selection of the list can be selected from the options menu and the last 
selected type will be stored and used in future.


--------------- Apps and tasks plug-in ------------------

This plug-in is used for getting information on different application installed into the device, 
as well as managing different UI applications that are currently running.

With the apps part, each installed application can be seen in a list that you can search with the search box provided in the bottom. 
Info menu option shows basic information of the application and the launch option allows launching the application.

The task parts then shown active UI application tasks. 
Show info menu option shows basic information and switch to allows bringing the application to the foreground. 
Close task can be used to send the task an end command, and Kill task can be used to force the task to exit by killing it.


--------------- Crash monitor plug-in ------------------

This plug-in is used for getting information on process exits in the device, and to get the process exit code. 
The actual monitoring is handled by background process, so it will be on even after the UI application is not running. 
The status of the background application is shown by the icon in the main selection list.

Normal application exit is shown as Kill-0-Kill, any other process exit reason will also show an exit note. If only exits 
with errors are wanted to be shown, then this mode of viewing can be selected from the options menu.

The show info menu option shows the time and basic information of the process exit. The list can be reset with Clear list option, 
and the background application can be shut down with Set off option.

--------------- FileTypes  plug-in ------------------

This plug-in is used for showing and managing the MIME type mapping to the handling applications. 
The list shows the current handler applications (if any) to the all MIME types defined for the device.

The menu option Map to new app can be used for changing the handler application. 
Note that some file types handler application can not be changed, and this is shown by not changing the handler applications name on the list.

The set to default mapping option can be used to set the default handler application back.


---------------  Fonts plug-in ------------------

This plug-in is used for showing the look and feel of all fonts available in the device.

The text for the fonts can e imported from a file, pasted from clipboard or entered with text dialog. 
The font can be changed with Change font option in the options menu.

System fonts include all non-scalable system fonts, logical fonts include normal scalable fonts used by the device, 
and the typeface font list includes all typefaces registered for the device. With typeface fonts the font height can also be set, 
and the default size for the font is 100.

--------------- Memory Status  plug-in ------------------

This plug-in is used for showing current memory status information for RAM and C,D and E-Drives as well as reserving space from these memories.

The compress RAM option tries to clear any memory that can be cleared, and reserve memory will reserve either RAM or disk space for the given amount. 
Note that you need to use free reservations menu options in order to clear any disk space reserved. 
RAM memory will be cleared latest on the re-boot of the device.

Note also that device caching might free some memory after you have reserved RAM memory, 
so try out compress RAM after each RAM memory reservations to see how much RAM can be freed.


--------------- Processes and Tasks plug-in ------------------

This plug-in is used for showing current processes and threads, their information as well as it offers options for terminating any 
process running on the device.

Show info shows extended information for the process/thread. And Kill options can be used for killing any process.

The re-fresh option can be used to re-fresh the process/threads list.


---------------  Trace plug-in ------------------

This plug-in is used for monitoring system CPU and memory usage. Note that the CPU value is not absolute value, 
but the smallest CPU usage level is used as a zero level.

There is also an always on pop-up window that can be set on and configured by selecting the settings menu item. 
Note that the font size determines the pop-up size, so adjust the value to the desired size for best usability. 
The pop-up when set on will be shown always while the background application is on.

The trace background application can be set on/off by the menu options and when set on it will also run after exiting the main application. 
The main applications selection list will show the background applications running status with different icons.

