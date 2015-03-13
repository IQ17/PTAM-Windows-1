# PTAM-Windows
PTAM-WIndows updated to VC 2013 and fixed PTGrey Capture Grasshopper3 USB

VS2013 has an issue with CC files and unix cr/lf styles keeping track in the debugger. adding CC as a C/C++ in th e registry might fix it, or just rename .cc to .cpp and it'll detect the cr/lf's properly.

TODO: Fix paths to non absolute