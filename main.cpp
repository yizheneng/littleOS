#include <QTimer>
#include <QDebug>
#include <QThread>
/****小小调度器开始**********************************************/
#define MAXTASKS 3
volatile unsigned char timers[MAXTASKS];
#define _SS static unsigned char _lc=0; switch(_lc){default:
#define _EE ;}; _lc=0; return 255;
#define WaitX(tickets)  do {_lc=(__LINE__+((__LINE__%256)==0))%256; return tickets ;} while(0); case (__LINE__+((__LINE__%256)==0))%256:

#define RunTask(TaskName,TaskID) do { if (timers[TaskID]==0) timers[TaskID]=TaskName(); }  while(0);
#define RunTaskA(TaskName,TaskID) { if (timers[TaskID]==0) {timers[TaskID]=TaskName(); continue;} }   //前面的任务优先保证执行

#define CallSub(SubTaskName) do {unsigned char currdt; _lc=(__LINE__+((__LINE__%256)==0))%256; return 0; case (__LINE__+((__LINE__%256)==0))%256:  currdt=SubTaskName(); if(currdt!=255) return currdt;} while(0);
#define InitTasks() {unsigned char i; for(i=MAXTASKS;i>0 ;i--) timers[i-1]=0; }
#define UpdateTimers() {unsigned char i; for(i=MAXTASKS;i>0 ;i--){if((timers[i-1]!=0)&&(timers[i-1]!=255)) timers[i-1]--;}}

#define SEM unsigned int
//初始化信号量
#define InitSem(sem) sem=0;
//等待信号量
#define WaitSem(sem) do{ sem=1; WaitX(0); if (sem>0) return 1;} while(0);
//等待信号量或定时器溢出， 定时器tickets 最大为0xFFFE
#define WaitSemX(sem,tickets)  do { sem=tickets+1; WaitX(0); if(sem>1){ sem--;  return 1;} } while(0);
//发送信号量
#define SendSem(sem)  do {sem=0;} while(0);

/*****小小调度器结束*******************************************************/

int sem0 = 0;
unsigned char task0(){
_SS
  while(1){
   WaitX(100);
   qDebug() << "Task0";
  }
_EE
}

unsigned char  task1(){
_SS
  while(1){
   WaitSem(sem0);
   qDebug() << "Task1";
  }
_EE
}

unsigned char  task2(){
_SS
    while(1){
        static int counter = 0;
        WaitX(100);
        if(counter >= 2) {
            counter = 0;
            SendSem(sem0);
        } else {
            counter ++;
        }
        qDebug() << "Task2";
    }
_EE
}

void InitT0()
{

}

void INTT0(void)
{
    UpdateTimers();
}


int main()
{
    InitT0();
    InitSem(sem0);
    InitTasks(); //初始化任务，实际上是给timers清零
    while(1){
        //           RunTask(task0,0);
        RunTask(task0,0);//任务0具有精确按时获得执行的权限，要求：task0每次执行消耗时间<0.5个 ticket
        RunTaskA(task1,1);//任务1具有比任务2高的运行权限
        RunTaskA(task2,2);//任务2具有低的运行权限
        INTT0();
        QThread::usleep(10000);
    }
}
