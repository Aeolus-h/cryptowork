#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 10000
#define SAMPLE_SIZE 100000000

// 均匀分布生成（推荐方式，避免rand()%N的偏差）
int uniform_rand(int n) {
    return (int)(rand() / (RAND_MAX + 1.0) * n);
}

// 正态分布生成（Box-Muller变换）
double normal_rand(double mu, double sigma) {
    double u1 = (rand() + 1.0) / (RAND_MAX + 2.0);
    double u2 = (rand() + 1.0) / (RAND_MAX + 2.0);
    double z0 = sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);
    return mu + sigma * z0;
}

// 1. 频数分布测试
void frequency_test(int (*rand_func)(int), int n, int sample_size, const char* label) {
    int* freq = (int*)calloc(n, sizeof(int));
    for (int i = 0; i < sample_size; ++i) {
        int x = rand_func(n);
        freq[x]++;
    }
    printf("\n[Frequency Test] %s\nValue\tFrequency\n", label);
    for (int i = 0; i < n; ++i) {
        printf("%d\t%d\n", i, freq[i]);
    }
    free(freq);
}

// 2. 间隔分布测试
void interval_test(int (*rand_func)(int), int n, int sample_size, const char* label) {
    int* last = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i) last[i] = -1;
    int interval_hist[100] = {0}; // 统计间隔在0~99
    for (int i = 0; i < sample_size; ++i) {
        int x = rand_func(n);
        if (last[x] != -1) {
            int interval = i - last[x];
            if (interval < 100) interval_hist[interval]++;
        }
        last[x] = i;
    }
    printf("\n[Interval Test] %s\nInterval\tCount\n", label);
    for (int i = 1; i < 100; ++i) {
        if (interval_hist[i] > 0)
            printf("%d\t\t%d\n", i, interval_hist[i]);
    }
    free(last);
}

// 3. 卡方检验
void chi_square_test(int (*rand_func)(int), int n, int sample_size, const char* label) {
    int* freq = (int*)calloc(n, sizeof(int));
    for (int i = 0; i < sample_size; ++i) {
        int x = rand_func(n);
        freq[x]++;
    }
    double expected = sample_size / (double)n;
    double chi2 = 0;
    for (int i = 0; i < n; ++i) {
        chi2 += (freq[i] - expected) * (freq[i] - expected) / expected;
    }
    printf("\n[Chi-square Test] %s\nChi-square value: %f\n", label, chi2);
    free(freq);
}

// 4. 自相关性分析
void autocorrelation_test(int (*rand_func)(int), int n, int sample_size, int lag, const char* label) {
    int* seq = (int*)malloc(sample_size * sizeof(int));
    for (int i = 0; i < sample_size; ++i) {
        seq[i] = rand_func(n);
    }
    double mean = 0;
    for (int i = 0; i < sample_size; ++i) mean += seq[i];
    mean /= sample_size;
    double num = 0, den = 0;
    for (int i = 0; i < sample_size - lag; ++i) {
        num += (seq[i] - mean) * (seq[i + lag] - mean);
    }
    for (int i = 0; i < sample_size; ++i) {
        den += (seq[i] - mean) * (seq[i] - mean);
    }
    printf("\n[Autocorrelation Test] %s\nAutocorrelation (lag %d): %f\n", label, lag, num / den);
    free(seq);
}

// 5. 游程检验
void runs_test(int (*rand_func)(int), int n, int sample_size, const char* label) {
    int prev = rand_func(n);
    int runs = 1;
    for (int i = 1; i < sample_size; ++i) {
        int curr = rand_func(n);
        if (curr != prev) runs++;
        prev = curr;
    }
    printf("\n[Runs Test] %s\nNumber of runs: %d\n", label, runs);
}

// rand()%N 生成器
int mod_rand(int n) {
    return rand() % n;
}

int main() {
    srand((unsigned int)time(NULL));

    // 1. 测试 rand()%N
    frequency_test(mod_rand, N, SAMPLE_SIZE, "rand()%N");
    interval_test(mod_rand, N, SAMPLE_SIZE, "rand()%N");
    chi_square_test(mod_rand, N, SAMPLE_SIZE, "rand()%N");
    autocorrelation_test(mod_rand, N, SAMPLE_SIZE, 1, "rand()%N");
    runs_test(mod_rand, N, SAMPLE_SIZE, "rand()%N");

    // 2. 测试改进的均匀分布
    frequency_test(uniform_rand, N, SAMPLE_SIZE, "uniform_rand");
    interval_test(uniform_rand, N, SAMPLE_SIZE, "uniform_rand");
    chi_square_test(uniform_rand, N, SAMPLE_SIZE, "uniform_rand");
    autocorrelation_test(uniform_rand, N, SAMPLE_SIZE, 1, "uniform_rand");
    runs_test(uniform_rand, N, SAMPLE_SIZE, "uniform_rand");

    // 3. 正态分布测试
    printf("\n[Normal Distribution Test] 10 samples (mean=0, std=1):\n");
    for (int i = 0; i < 10; ++i) {
        printf("%f ", normal_rand(0, 1));
    }
    printf("\n");

    return 0;
}
