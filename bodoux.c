#include "bodoux.h"
#include <stdint.h>

typedef struct bodoux_weights_t {
	uint64_t* keys;
	int16_t* values;
	int32_t count;
} bodoux_weights_t;

typedef struct bodoux_model_t  {
	int32_t base_score;
	bodoux_weights_t UW1;
	bodoux_weights_t UW2;
	bodoux_weights_t UW3;
	bodoux_weights_t UW4;
	bodoux_weights_t UW5;
	bodoux_weights_t UW6;
	bodoux_weights_t BW1;
	bodoux_weights_t BW2;
	bodoux_weights_t BW3;
	bodoux_weights_t TW1;
	bodoux_weights_t TW2;
	bodoux_weights_t TW3;
	bodoux_weights_t TW4;
} bodoux_model_t;

// Models
#include "model_ja.h"
#include "model_zh_hans.h"
#include "model_zh_hant.h"
#include "model_th.h"


// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

static uint32_t decutf8(uint32_t* state, uint32_t* codep, uint8_t byte)
{
	static const uint8_t utf8d[] = {
		// The first part of the table maps bytes to character classes that
		// to reduce the size of the transition table and create bitmasks.
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

		// The second part is a transition table that maps a combination
		// of a state of the automaton and a character class to a state.
		0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
		12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
		12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
		12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
		12,36,12,12,12,12,12,12,12,12,12,12,
	};

	uint32_t type = utf8d[byte];

	*codep = (*state != UTF8_ACCEPT) ?
		(byte & 0x3fu) | (*codep << 6) :
		(0xff >> type) & (byte);

	*state = utf8d[256 + *state + type];
	return *state;
}

struct utf8_iter_t;

typedef uint32_t (*get_func_t)(struct utf8_iter_t* iter);

typedef struct utf8_iter_t {
	const char* buf;
	int32_t pos;
	int32_t done;
	int32_t empty_count;
	get_func_t get;
} utf8_iter_t;

static uint32_t get_empty(utf8_iter_t* iter)
{
	iter->empty_count--;
	if (iter->empty_count <= 0) 
		iter->done = 1;
	return 0;    
}

static uint32_t get_value(utf8_iter_t* iter)
{
	uint32_t state = 0;
	uint32_t cp = 0;

	// Parse utf-8 code point
	decutf8(&state, &cp, iter->buf[iter->pos++]);
	if (state && iter->buf[iter->pos])
		decutf8(&state, &cp, iter->buf[iter->pos++]);
	if (state && iter->buf[iter->pos])
		decutf8(&state, &cp, iter->buf[iter->pos++]);
	if (state && iter->buf[iter->pos])
		decutf8(&state, &cp, iter->buf[iter->pos++]);
	if (state != UTF8_ACCEPT)
		cp = 0;

	// If reached end of string, start emitting empty code points.
	if (iter->buf[iter->pos] == '\0')
		iter->get = &get_empty;

	return cp;
}

utf8_iter_t make_utf8_iter(const char* buf, int32_t empty_count)
{
	return (utf8_iter_t) {
		.buf = buf,
		.pos = 0,
		.empty_count = empty_count,
		.done = 0,
		.get = (buf && buf[0]) ? &get_value : &get_empty,
	};
}

static int32_t find_weight(const bodoux_weights_t* w, const uint64_t key)
{
	if (key < w->keys[0] || key > w->keys[w->count-1])
		return 0;

	// binary search
	int32_t low = 0;
	int32_t high = w->count - 1;
	while (low != high) {
		const int32_t mid = low + (high - low + 1) / 2; // ceil
		if (w->keys[mid] > key)
			high = mid - 1;
		else
			low = mid;
	}
	if (w->keys[low] == key)
		return w->values[low];
	
	return 0;
}
	
static int32_t find_uni_weight(const bodoux_weights_t* w, const uint64_t cp0)
{
	return find_weight(w, cp0);
}

static int32_t find_bi_weight(const bodoux_weights_t* w, const uint64_t cp0, const uint64_t cp1)
{
	return find_weight(w, cp0 | (cp1 << 20));
}

static int32_t find_tri_weight(const bodoux_weights_t* w, const uint64_t cp0, const uint64_t cp1, const uint64_t cp2)
{
	return find_weight(w, cp0 | (cp1 << 20) | (cp2 << 40));
}


static void parse_boundaries(const bodoux_model_t* model, const char* text, char* boundaries)
{
	utf8_iter_t iter = make_utf8_iter(text, 2);

	uint32_t buffer[6] = { 0 };
	int32_t offset[6] = { 0 };
	#define ROLL(a) \
		(a)[0] = (a)[1]; \
		(a)[1] = (a)[2]; \
		(a)[2] = (a)[3]; \
		(a)[3] = (a)[4]; \
		(a)[4] = (a)[5]
	
	// prebuffer
	for (int32_t i = 0; i < 2; i++) {
		ROLL(offset);
		offset[5] = iter.pos;
		ROLL(buffer);
		buffer[5] = iter.get(&iter);
	}
	
	while (!iter.done) {
		ROLL(offset);
		offset[5] = iter.pos;
		ROLL(buffer);
		buffer[5] = iter.get(&iter);

		// buffer[0] is -3, buffer[3] = 0, buffer[5] = +2;
		int score = model->base_score;
		score += find_uni_weight(&model->UW1, buffer[0]);
		score += find_uni_weight(&model->UW2, buffer[1]);
		score += find_uni_weight(&model->UW3, buffer[2]);
		score += find_uni_weight(&model->UW4, buffer[3]);
		score += find_uni_weight(&model->UW5, buffer[4]);
		score += find_uni_weight(&model->UW6, buffer[5]);
		score += find_bi_weight(&model->BW1, buffer[1],buffer[2]);
		score += find_bi_weight(&model->BW2, buffer[2],buffer[3]);
		score += find_bi_weight(&model->BW3, buffer[3],buffer[4]);
		score += find_tri_weight(&model->TW1, buffer[0],buffer[1],buffer[2]);
		score += find_tri_weight(&model->TW1, buffer[1],buffer[2],buffer[3]);
		score += find_tri_weight(&model->TW1, buffer[2],buffer[3],buffer[4]);
		score += find_tri_weight(&model->TW1, buffer[3],buffer[4],buffer[5]);
		if (score > 0)
			boundaries[offset[3]] = 1;
	}
}

void parse_boundaries_ja(const char* text, char* boundaries)
{
	parse_boundaries(&model_ja, text, boundaries);
}

void parse_boundaries_zh_hans(const char* text, char* boundaries)
{
	parse_boundaries(&model_zh_hans, text, boundaries);
}

void parse_boundaries_zh_hant(const char* text, char* boundaries)
{
	parse_boundaries(&model_zh_hant, text, boundaries);
}

void parse_boundaries_th(const char* text, char* boundaries)
{
	parse_boundaries(&model_th, text, boundaries);
}
