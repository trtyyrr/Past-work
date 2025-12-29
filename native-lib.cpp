#include <jni.h>
#include <thread>
#include <atomic>
#include <vector>

using namespace std;

atomic<long long> global_pts(0);
atomic<bool> is_running(false);

extern "C" JNIEXPORT void JNICALL
Java_com_nexus_audit_MainActivity_startAudit(JNIEnv* env, jobject /* this */, jint threads) {
    is_running = true;
    for (int i = 0; i < threads; i++) {
        thread([]() {
            while (is_running) {
                // 1000x1000 深度嵌套汇编块
                for (int j = 0; j < 1000; j++) {
                    asm volatile (
                        "mov w0, #1000\n\t"
                        "1:\n\t"
                        "fadd v0.4s, v0.4s, v1.4s\n\t" // 向量加法
                        "fmla v2.4s, v0.4s, v1.4s\n\t" // 向量乘加
                        "fsqrt v3.4s, v2.4s\n\t"       // 向量开方
                        "frsqrte v4.4s, v3.4s\n\t"     // 倒数平方根近似
                        "subs w0, w0, #1\n\t"
                        "b.ne 1b\n\t"
                        : : : "v0","v1","v2","v3","v4","w0","cc"
                    );
                }
                global_pts.fetch_add(1);
            }
        }).detach();
    }
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_nexus_audit_MainActivity_getPts(JNIEnv* env, jobject /* this */) {
    return global_pts.load();
}