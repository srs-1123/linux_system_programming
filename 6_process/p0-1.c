#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/* コマンドライン引数、環境変数のアドレス保持用 */
int *Argc;
char *** Argv;
char ***Envp;

void ReExec()
{
    /* sleep()によるSIGALRM を解除:Linuxでは行わなくてもよいが念のため */
    alarm(0);

    /* 連続シグナル */
    signal(SIGHUP, SIG_IGN);

    /* 自プロセスの上書き再実行 */
    if(execve((*Argv)[0], (*Argv), (*Envp))==-1){
        perror("execve");
    }
}

void HupHandler(int sig)
{
    fprintf(stderr, "HupHandler(signal=%d)/n", sig);

    ReExec();
}

int main(int argc, char *argv[], char *envp[])
{
    struct sigaction sa;
    int i;

    /* コマンドライン引数、環境変数のアドレスをグローバルに保持 */
    Argc=&argc;
    Argv=&argv;
    Envp=&envp;

    /* コマンドライン引数、環境変数の表示*/

    /* SIGHUPのシグナルハンドラを指定 */
#ifdef USE_SIGNAL
    signal(SIGHUP, HupHandler); // Linuxではうまくいかない

    /* 現状の表示 */
    sigaction(SIGHUP, (struct sigaction *)NULL, &sa);
    fprintf(stderr, "SA_ONSTACKf=%d\n", (sa.sa_flags&SA_ONSTACK)?1:0);
    fprintf(stderr, "SA_RESETHAND=%d\n",(sa.sa_flags&SA_RESETHAND)?1:0);
    fprintf(stderr, "SA_RESETHAND=%d\n",(sa.sa_flags&SA_NODEFER)?1:0);
    fprintf(stderr, "SA_nodefer=%d\n", (sa.sa_flags&SA_RESTART)?1:0);
    fprintf(stderr, "SA_SIGINFO=%d\n", (sa.sa_flags&SA_SIGINFO)?1:0);
    for(i=1;i<=31;i++){
        fprintf(stderr, "%d=%d\n", i, sigismember(&sa.sa_mask, i));
    }
    fprintf(stderr, "signal():end\n");
#else
    sigaction(SIGHUP, (struct sigaction *)NULL, &sa);
    sa.sa_handler=HupHandler;
    sa.sa_flags=SA_NODEFER; // それ自身のシグナルハンドラの内部にいるときでもシグナルを妨害しないようにする
    sigaction(SIGHUP, &sa, (struct sigaction *)NULL);

    /* 現状の表示 */
    sigaction(SIGHUP, (struct sigaction *)NULL, &sa);
    fprintf(stderr, "sigaction():end\n");
    fprintf(stderr, "SA_ONSTACK=%d\n", (sa.sa_flags&SA_ONSTACK)?1:0);
    fprintf(stderr, "SA_SIGINFO=%d\n", (sa.sa_flags&SA_RESETHAND)?1:0);
    fprintf(stderr, "SA_RESETHAND=%d\n", (sa.sa_flags&SA_NODEFER)?1:0);
    fprintf(stderr, "SA_NODEFER=%d\n", (sa.sa_flags&SA_RESTART)?1:0);
    fprintf(stderr, "SA_RESTART=%d\n", (sa.sa_flags&SA_SIGINFO)?1:0);
    for(i=1;i<=31;i++){
        fprintf(stderr, "%d=%d\n", i, sigismember(&sa.sa_mask, i));
    }
#endif
    /* 5秒おきにカウント表示 */
    for(i=0; ;i++){
        fprintf(stderr, "count=%d\n", i);
        sleep(5);
    }
}