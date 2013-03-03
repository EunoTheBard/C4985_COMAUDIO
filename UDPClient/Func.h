#ifndef FUNC_H_
#define FUNC_H_

int pow(int, int);
int waveIn(char *, LPHWAVEOUT, LPWAVEFORMATEX, LPWAVEHDR);
LRESULT CALLBACK TrackProc(HWND, UINT, WPARAM, LPARAM);

#endif