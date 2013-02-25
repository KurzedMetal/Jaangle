#ifndef _IconSerialization_h_
#define _IconSerialization_h_

class IconSerialization
{
public:
	static HICON LoadIcon(LPCSTR iconDescription);
	static LPSTR SaveIcon(HICON hIcon);
};

#endif