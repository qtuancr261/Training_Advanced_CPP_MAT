/*
 * Author: tuantq3
 * File: %{Cpp:License:FileName}
 *
 * Created on 12/7/2020
 */
#include <lz4.h>
#include <snappy-c.h>
#include <snappy.h>
#include <zdict.h>
#include <zstd.h>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include "google_benchmark/inc/benchmark.h"
using namespace std;
std::default_random_engine randEngine;
void zenString(std::string& result, size_t len, bool isPrintable) {
	int min = isPrintable ? 32 : 0;
	int max = isPrintable ? 126 : 255;
	std::uniform_int_distribution<char> dis(min, max);
	result.resize(len);
	for (int i{}; i < len; ++i) {
		result[i] = (dis(randEngine));
	}
}
void readMsgTemplate(std::string& msg) {
	QFile msgTempFile("zns_msgp.245111.html");
	if (msgTempFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream readStream(&msgTempFile);
		QString line("");
		while (readStream.readLineInto(&line)) {
			// qDebug() << line;
			msg.append(line.toStdString());
		}
	}
}
static void BM_StringCreation(benchmark::State& state) {
	for (auto _ : state) std::string empty_string;
}

// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

static void BM_StringCopy(benchmark::State& state) {
	std::string x = "hello";
	for (auto _ : state) std::string copy(x);
}
BENCHMARK(BM_StringCopy);

// memcpy as default ratio
static void BM_memcpy(benchmark::State& state) {}

// Begin benchmark
// BENCHMARK_MAIN();
std::string example(1000, 'z');
std::string target(4096, 'a');
std::string decompressTarget(1000, 'b');
int sizeCompress{};
int sizeDecompress{};
int sizeCompressBound{};

static void BM_LZ4_compress_default(benchmark::State& state) {
	example.clear();
	readMsgTemplate(example);
	sizeCompressBound = LZ4_compressBound(example.size());
	target.resize(sizeCompressBound);
	for (auto _ : state) {
		sizeCompress = LZ4_compress_default(example.data(), target.data(), example.size(), target.size());
	}
}

BENCHMARK(BM_LZ4_compress_default);

static void BM_LZ4_decompress_safe(benchmark::State& state) {
	// 4kB
	// cout << "size " << sizeCompress;
	decompressTarget.resize(example.size());
	target.resize(sizeCompress);
	for (auto _ : state) {
		sizeDecompress = LZ4_decompress_safe(target.data(), decompressTarget.data(), target.size(), decompressTarget.size());
	}
	// cout << "Size decomp " << sizeDecompress << endl;
}

BENCHMARK(BM_LZ4_decompress_safe);

static void BM_snappy_compress(benchmark::State& state) {
	example.clear();
	readMsgTemplate(example);
	target.clear();
	for (auto _ : state) {
		sizeCompress = snappy::Compress(example.data(), example.size(), &target);
	}
}

BENCHMARK(BM_snappy_compress);

static void BM_snappy_decompress(benchmark::State& state) {
	decompressTarget.clear();
	for (auto _ : state) {
		sizeDecompress = snappy::Uncompress(target.data(), target.size(), &decompressTarget);
	}
}

BENCHMARK(BM_snappy_decompress);

static void BM_zstd_compress(benchmark::State& state) {
	example.clear();
	readMsgTemplate(example);
	target.clear();
	sizeCompressBound = ZSTD_compressBound(example.size());
	target.resize(sizeCompressBound);
	for (auto _ : state) {
		sizeCompress = ZSTD_compress(target.data(), sizeCompressBound, example.data(), example.size(), 1);
	}
}

BENCHMARK(BM_zstd_compress);

static void BM_zstd_decompress(benchmark::State& state) {
	decompressTarget.clear();
	decompressTarget.resize(ZSTD_getFrameContentSize(target.data(), target.size()));
	for (auto _ : state) {
		sizeDecompress = ZSTD_decompress(decompressTarget.data(), decompressTarget.size(), target.data(), target.size());
	}
}

BENCHMARK(BM_zstd_decompress);

static void BM_zstd_compress_context(benchmark::State& state) {
	// same compress ratio with zstd_compress
	// optimize speed / resource
	example.clear();
	readMsgTemplate(example);
	target.clear();
	ZSTD_CCtx* comContext = ZSTD_createCCtx();
	sizeCompressBound = ZSTD_compressBound(example.size());
	target.resize(sizeCompressBound);
	for (auto _ : state) {
		// sizeCompress = ZSTD_compress((void*)target.data(), target.size(), (void*)example.data(), example.size(), 1);
		sizeCompress = ZSTD_compressCCtx(comContext, target.data(), sizeCompressBound, example.data(), example.size(), 1);
	}
	ZSTD_freeCCtx(comContext);
}

BENCHMARK(BM_zstd_compress_context);

static void BM_zstd_decompress_context(benchmark::State& state) {
	// same decompress ratio with zstd_decompress
	// optimize speed / resource
	decompressTarget.clear();
	decompressTarget.resize(ZSTD_getFrameContentSize(target.data(), target.size()));
	ZSTD_DCtx* decomContext = ZSTD_createDCtx();
	for (auto _ : state) {
		// sizeDecompress = ZSTD_decompress(decompressTarget.data(), decompressTarget.size(), target.data(), target.size());
		sizeDecompress = ZSTD_decompressDCtx(decomContext, decompressTarget.data(), decompressTarget.size(), target.data(), target.size());
	}
	ZSTD_freeDCtx(decomContext);
}

BENCHMARK(BM_zstd_decompress_context);

BENCHMARK_MAIN();
// int main() {
//	example.clear();
//	cout << "LZ4" << endl;
//	readMsgTemplate(example);
//	cout << example.size() << endl;
//	sizeCompressBound = LZ4_compressBound(example.size());
//	cout << "Bound size: " << sizeCompressBound << endl;
//	target.resize(sizeCompressBound);
//	cout << target.size() << endl;
//	sizeCompress = LZ4_compress_default(example.data(), target.data(), example.size(), target.size());
//	target.resize(sizeCompress);
//	cout << sizeCompress << endl;
//	decompressTarget.resize(example.size());
//	sizeDecompress = LZ4_decompress_safe(target.data(), decompressTarget.data(), target.size(), decompressTarget.size());
//	cout << sizeDecompress << endl;

//	cout << "Snappy " << SNAPPY_VERSION << endl;
//	cout << snappy::Compress(example.data(), example.size(), &target) << endl;
//	cout << snappy::Uncompress(target.data(), target.size(), &decompressTarget) << endl;
//	cout << decompressTarget.size() << endl;
//	assert(example == decompressTarget);

//	cout << "ZSTD " << ZSTD_versionNumber() << endl;
//	sizeCompressBound = ZSTD_compressBound(example.size());
//	cout << "Bound size " << sizeCompressBound << endl;
//	target.resize(sizeCompressBound);
//	sizeCompress = ZSTD_compress((void*)target.data(), target.size(), (void*)example.data(), example.size(), 1);
//	cout << "Comp size " << sizeCompress << endl;
//	cout << "decomp size " << ZSTD_getFrameContentSize(target.data(), target.size()) << endl;
//	target.resize(sizeCompress);
//	sizeDecompress = ZSTD_decompress((void*)decompressTarget.data(), decompressTarget.size(), (void*)target.data(), target.size());
//	cout << "Decomp size " << sizeDecompress << endl;
//	assert(example == decompressTarget);

//	cout << "ZSTD use context " << ZSTD_versionNumber() << endl;
//	ZSTD_CCtx* comCtx = ZSTD_createCCtx();
//	sizeCompressBound = ZSTD_compressBound(example.size());
//	cout << "Bound size " << sizeCompressBound << endl;
//	target.resize(sizeCompressBound);
//	sizeCompress = ZSTD_compressCCtx(comCtx, target.data(), target.size(), example.data(), example.size(), 1);
//	cout << "Comp size " << sizeCompress << endl;
//	cout << "decomp size " << ZSTD_getFrameContentSize(target.data(), target.size()) << endl;
//	ZSTD_DCtx* decomCtx = ZSTD_createDCtx();
//	target.resize(sizeCompress);
//	sizeDecompress = ZSTD_decompressDCtx(decomCtx, decompressTarget.data(), sizeDecompress, target.data(), target.size());
//	cout << "Decomp size " << sizeDecompress << endl;
//	assert(example == decompressTarget);

//	return 0;
//}