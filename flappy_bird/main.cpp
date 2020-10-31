#include <graphics.h>  //图形界面库
#include<easyx.h>
#include<stdio.h>
#include<mmsystem.h> 
#include<time.h>

//Windows Multimedia API
#pragma comment(lib,"winmm.lib")

IMAGE memimg; //缓冲图层
IMAGE backimg;//背景图片
IMAGE birdimg[3];
IMAGE landimg;
IMAGE pipeimg[2];

int temp0;
int temp1;
int bird_x, bird_y,bird_f;
int vy, g;//小鸟
DWORD t1, t2;//制造时间间隔控制速度
int pipe_x[2], pipe_y[2];
int pipe_up; 
int pipe_down; 
int vx;  //管道
int land_x 
;
int land_y ;
int exit_flag;

void drawAlpha(IMAGE* dstimg, int x, int y, IMAGE* srcimg);//绘图
void game_init();//游戏初始化
void game_draw(); //游戏的绘制
void game_logic();//游戏的逻辑
void game_play();//操控
void bird_drop();
void if_bump();//判断小鸟是否撞杆
void pipeLand_roll();
void game_begin();



//根据透明度绘图
//dstimg: 目标文件
//srcimg: 源文件
void drawAlpha(IMAGE* dstimg, int x, int y, IMAGE* srcimg) {
    if (dstimg == NULL) {
        return;
    }

    DWORD* dst = GetImageBuffer(dstimg);// 显存
    DWORD* src = GetImageBuffer(srcimg);
    int src_width = srcimg->getwidth();
    int src_height = srcimg->getheight();
    int dst_width = dstimg->getwidth();
    int dst_height = dstimg->getheight();

    //实现透明化贴图 通过像素操作，把每个像素点都拿出来
    for (int iy = 0; iy < src_height; iy++) {
        for (int ix = 0; ix < src_width; ix++) {
            int srcX = ix + iy * src_width;
            //把每个像素点的红绿蓝拆开
            int sa = ((src[srcX] & 0xff000000) >> 24);//Alpha透明度
            int sr = ((src[srcX] & 0xff0000) >> 16); //red
            int sg = ((src[srcX] & 0xff00) >> 8);    //green
            int sb = src[srcX] & 0xff;               //blue
            if (x + ix >= 0 && x + ix < dst_width
                && y + iy >= 0 && y + iy < dst_height) {
                int dstX = (x + ix) + (y + iy) * dst_width;
                int dr = ((dst[dstX] & 0xff0000) >> 16);
                int dg = ((dst[dstX] & 0xff00) >> 8);
                int db = dst[dstX] & 0xff;
                //贝叶斯定律
                dst[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)
                    | ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)
                    | (sb * sa / 255 + db * (255 - sa) / 255);
            }
        }
    }
}


//加载图片，游戏初始化
void game_init() {
    loadimage(&backimg, L"back.png");
    loadimage(&memimg, L"back.png"); //画布
    loadimage(&birdimg[0], L"bird0_0.png");
    loadimage(&birdimg[1], L"bird0_1.png");
    loadimage(&birdimg[2], L"bird0_2.png");
    loadimage(&landimg, L"land.png");
    loadimage(&pipeimg[0], L"pipe_down.png");
    loadimage(&pipeimg[1], L"pipe_up.png");

    mciSendString(_T("open Croatian.mp3"), 0, 0, 0);
    mciSendString(_T("play Croatian.mp3"), 0, 0, 0);

    bird_x = 40;
    bird_y = 100;
    bird_f = 0;
    vy = 0;
    g = 1;
    t1 = GetTickCount();//获取系统的时间

    pipe_x[0] = 288 + 30;
    pipe_x[1] = 288 + 30 + 190;
    pipe_y[0] = rand() % 250;
    pipe_y[1] = rand() % 250;

    land_x = 0;
    land_y = 430;

    pipe_up = -280;
    pipe_down = 160;
    vx = 1; //管道

    exit_flag = 0;

}


void game_begin() {
    
        drawAlpha(&memimg, 0, 0, &backimg);
        drawAlpha(&memimg, 130, 240, &birdimg[bird_f]);
        if (++bird_f >= 3) {
            bird_f = 0;
        }
        drawAlpha(&memimg, land_x, land_y, &landimg);
        land_x -= vx;
        if (land_x <= -30) {
            land_x = 0;
        }
        
        
    

}


void game_draw() {
    //画背景  backimg --> memimg
    drawAlpha(&memimg, 0, 0, &backimg);

    //画管道
    drawAlpha(&memimg, pipe_x[0], pipe_y[0] + pipe_up, &pipeimg[0]);
    drawAlpha(&memimg, pipe_x[0], pipe_y[0] + pipe_down, &pipeimg[1]);
    drawAlpha(&memimg, pipe_x[1], pipe_y[1] + pipe_up, &pipeimg[0]);
    drawAlpha(&memimg, pipe_x[1], pipe_y[1] + pipe_down, &pipeimg[1]);

    //画小鸟
    drawAlpha(&memimg, bird_x, bird_y, &birdimg[bird_f]);

    //画地面
    drawAlpha(&memimg, land_x, land_y, &landimg);

    //了断   memimg
    putimage(0, 0, &memimg);
}


void  bird_drop() {
    if (++bird_f >= 3) {
        bird_f = 0;
    }
    t2 = GetTickCount();
    if (t2 - t1 > 85) {  //ms
        vy += g;
        t1 = t2;
    }

    bird_y += vy;
}



void pipeLand_roll() {
    pipe_x[0] -= vx;
    pipe_x[1] -= vx;
    land_x -= vx;

    if (pipe_x[0] <= -52) {
        pipe_x[0] = pipe_x[1] + 190;
        pipe_y[0] = rand() % 250;
    }
    if (pipe_x[1] <= -52) {
        pipe_x[1] = pipe_x[0] + 190;
        pipe_y[1] = rand() % 250;
    }
    if (land_x <= -30) {
        land_x = 0;
    }
}



void if_bump()
{
    if (bird_x + 30 >= pipe_x[0] && bird_x <= pipe_x[0] + 45) {
        if (bird_y<pipe_y[0] + pipe_up + 300 || bird_y>pipe_y[0] + 270 + pipe_up + pipe_down) {
            exit_flag = 1;
        }
    }
    if (bird_x + 30 >= pipe_x[1] && bird_x <= pipe_x[1] + 45) {
        if (bird_y<pipe_y[1] + pipe_up + 300 || bird_y>pipe_y[1] + 270 + pipe_up + pipe_down) {
            exit_flag = 1;
        }
    }

}

void game_logic()
{
    bird_drop();
    pipeLand_roll();
    if_bump();
}


void game_play()
{
    //改变一些数据，实现游戏效果
    MOUSEMSG msg;//鼠标信息
    while (MouseHit()) {  //
        msg = GetMouseMsg();//获取鼠标信息
        if (msg.uMsg == WM_LBUTTONDOWN) {//如果得到鼠标左键的点击
            vy = -3;
            //mciSendString(_T("open click.mp3"), 0, 0, 0);
            //mciSendString(_T("play click.mp3"), 0, 0, 0);
        }
    }
}

int main()
{
    
    srand(time(0));
    initgraph(288, 512);  //窗口长288，高512像素
    game_init();
    while (1) {
        game_draw(); //各种数据的绘制
        game_logic();//数据的变化
        game_play();
        Sleep(8);
        if (exit_flag) {
            break;
        }

    }
    while (1) {
        game_draw();
        bird_drop();
    }
    getchar();
    return 0;
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
