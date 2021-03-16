#include <iostream>
#include <sdl/SDL.h>
using namespace std;

#pragma comment(lib, "SDL2.lib")
#undef main

int main(int argc, char* argv[]) {
	int w = 800;
	int h = 600;
	//1.��ʼ��SDL video��
	if (SDL_Init(SDL_INIT_VIDEO)) {
		cout << "SDL_GetError()" << endl;
		return -1;
	}

	cout << "SDL_Init success!" << endl;

	//2.����SDL ����
	auto screen = SDL_CreateWindow("test sdl ffmpeg",
		SDL_WINDOWPOS_CENTERED,//����λ��
		SDL_WINDOWPOS_CENTERED,
		w, h,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	if (!screen) {
		cout << "SDL_GetError()" << endl;
		return -2;
	}

	//3.������Ⱦ��
	auto render = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
	if (!render) {
		cout << SDL_GetError() << endl;
		return -3;
	}

	//4.���ɲ���
	auto texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,//�ɼ���
		w, h);
	if (!texture) {
		cout << SDL_GetError() << endl;
		return -4;
	}

	//���ͼ�������
	shared_ptr<unsigned char> rgb(new unsigned char[w * h * 4]);
	auto r = rgb.get();
	unsigned char tmp = 255;

	for (;;) {
		//�ж��˳�
		SDL_Event ev;
		SDL_WaitEventTimeout(&ev, 10);
		if (ev.type == SDL_QUIT) {
			SDL_DestroyWindow(screen);
			break;
		}

		tmp--;

		for (int j = 0; j < h; j++) {
			int b = j * w * 4;
			for (int i = 0; i < w * 4; i += 4) {
				r[b + i] = 0;		 //B
				r[b + i + 1] = 0;    //G
				r[b + i + 2] = tmp;	 //R
				r[b + i + 3] = 0;	 //A
			}
		}

		//5 ���ڴ�����д�����
		SDL_UpdateTexture(texture, NULL, r, w * 4);

		//6 ������Ļ
		SDL_RenderClear(render);

		//7 ���Ʋ��ʵ���Ⱦ����
		SDL_Rect sdl_rect;
		sdl_rect.x = 0;
		sdl_rect.y = 0;
		sdl_rect.w = w;
		sdl_rect.h = h;
		SDL_RenderCopy(render, texture,
			NULL,//ԭͼλ�úͳߴ�
			&sdl_rect//Ŀ��λ�úͳߴ�
		);

		//8.��Ⱦ
		SDL_RenderPresent(render);
	}
		
	getchar();
	return 0;
}