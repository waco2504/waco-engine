#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>

#include "../engine_main/OBJLoader.hpp"

class Writer {
	HANDLE hThread;
	bool loaded;
	OBJLoader ol;
	OBJLOADER_OBJECT ob;
	unsigned int fs;
public:
	static DWORD WINAPI ThreadProc_s(void* ptr) {
		return ((Writer*)ptr)->ThreadProc();
	}

	
	Writer() {
		loaded = false;
		fs = 0;
		hThread = CreateThread(0, 0, &Writer::ThreadProc_s, this, 0, 0);
	}
	~Writer() {
		TerminateThread(hThread, 0);
	}

	DWORD ThreadProc() {
		ol.loadObjectFromOBJ(&ob, "C:\\Users\\waco\\Desktop\\engine\\Debug\\Sponza.obj");
		
		for(unsigned int i = 0; i < ob.groups.size(); ++i) {
			fs += ob.groups[i].faces.size();
		}
		loaded = true;
		return 0;
	}

	bool isLoaded() const {
		return loaded;
	}

	unsigned int getFaceCount() const {
		return fs;
	}

	const OBJLOADER_OBJECT getOb() const {
		return ob;
	}
};

const int T1[] = {1,2,3,0};
const int T2[] = {1,2,3,4,0};
const int T3[] = {1,2,0};

const int* Tab[3] = { T1, T2, T3 };


int main(int argc, char* argv[]) {
	/*unsigned int anim = 0;

	Writer w; // odpal watek zaladuj plik

	char c = 0;
	while(true) {
		switch(anim) {
			case 0:
				c = '|';
				break;
			case 1:
				c = '/';
				break;
			case 2:
				c = '-';
				break;
			case 3:
				c = '\\'; 
				break;
		};
		++anim;
		if(anim > 3) anim = 0;
		
		if(w.isLoaded()) {
			printf("DONE!");
			printf("Wczytano facow: %u\n", w.getFaceCount());
			getchar();
			break;
		}
		
		system("cls");
		printf("%c\n", c);
		Sleep(300);
	}*/

	



	system("PAUSE");
	return 0;
}