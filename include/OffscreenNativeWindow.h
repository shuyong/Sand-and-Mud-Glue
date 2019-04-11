#ifndef _OFFSCREENNATIVEWINDOW_H_
#define _OFFSCREENNATIVEWINDOW_H_

typedef struct OffscreenNativeWindow OffscreenNativeWindow;

/* running on glibc space */
OffscreenNativeWindow* OffscreenNativeWindowCtor(int width, int height, int format, int usage);
void OffscreenNativeWindowDtor(OffscreenNativeWindow* window);
void OffscreenNativeWindowResize(OffscreenNativeWindow* window, int width, int height, int format);
ANativeWindow* OffscreenNativeWindowGetNativeWindow(OffscreenNativeWindow* window);

#endif /*_OFFSCREENNATIVEWINDOW_H_*/
