#include "circbuf.h"

#include "../../external/unity/unity.h"

typedef struct {
	int a;
	int b;
} TestStruct;

#define CB_CAPACITY 16
DECLARE_CIRCBUF(test_cb, TestStruct);
typedef struct test_cb CircBuf;

CircBuf	   cb;
TestStruct buf[CB_CAPACITY];

void setUp(void) {
	circbuf_init(&cb, buf, CB_CAPACITY);
}

void tearDown(void) {
	circbuf_clear(&cb);
}

TestStruct test_struct	= {1, 2};
TestStruct test_struct2 = {3, 4};

void test_cb_init(void) {
	TEST_ASSERT_EQUAL(cb.capacity, CB_CAPACITY);
	TEST_ASSERT_EQUAL(0, cb.in);
	TEST_ASSERT_EQUAL(0, cb.out);
}

void test_cb_alloc(void) {
	CircBuf cb_malloc;
	TEST_ASSERT_TRUE(circbuf_alloc(&cb_malloc, TestStruct, CB_CAPACITY));

	TEST_ASSERT_EQUAL(cb_malloc.capacity, CB_CAPACITY);
	TEST_ASSERT_EQUAL(0, cb_malloc.in);
	TEST_ASSERT_EQUAL(0, cb_malloc.out);
	TEST_ASSERT_NOT_NULL(cb_malloc.data);
	circbuf_free(&cb_malloc);
}

void test_cb_put(void) {
	circbuf_put(&cb, test_struct);
	TEST_ASSERT_EQUAL(1, cb.in);
	TEST_ASSERT_EQUAL(0, cb.out);
	TEST_ASSERT_EQUAL(1, cb.data[0].a);
	TEST_ASSERT_EQUAL(2, cb.data[0].b);
}

void test_cb_len(void) {
	size_t len = circbuf_len(&cb);
	TEST_ASSERT_EQUAL(0, len);

	circbuf_put(&cb, test_struct);
	len = circbuf_len(&cb);
	TEST_ASSERT_EQUAL(1, len);

	circbuf_put(&cb, test_struct2);
	len = circbuf_len(&cb);
	TEST_ASSERT_EQUAL(2, len);
}

void test_cb_clear(void) {
	circbuf_put(&cb, test_struct);
	circbuf_put(&cb, test_struct2);
	circbuf_clear(&cb);
	TEST_ASSERT_EQUAL(0, cb.in);
	TEST_ASSERT_EQUAL(0, cb.out);
}

void test_cb_get(void) {
	circbuf_put(&cb, test_struct);
	circbuf_put(&cb, test_struct2);

	TestStruct out;
	int		   ret = circbuf_get(&cb, &out);
	TEST_ASSERT_EQUAL(ret, 1);
	TEST_ASSERT_EQUAL(out.a, 1);
	TEST_ASSERT_EQUAL(out.b, 2);

	size_t len = circbuf_len(&cb);
	TEST_ASSERT_EQUAL(1, len);
}

void test_cb_empty(void) {
	TEST_ASSERT_TRUE(circbuf_empty(&cb));
	TEST_ASSERT_EQUAL(0, circbuf_len(&cb));

	circbuf_put(&cb, test_struct);
	TEST_ASSERT_EQUAL(1, circbuf_len(&cb));
	TEST_ASSERT_FALSE(circbuf_empty(&cb));
}

void test_cb_full(void) {
	TEST_ASSERT_FALSE(circbuf_full(&cb));
	for (int i = 0; i < CB_CAPACITY - 1; i++) {
		TEST_ASSERT_TRUE(circbuf_put(&cb, test_struct));
	}
	TEST_ASSERT_TRUE(circbuf_full(&cb));
	// our circular buffer is designed to hold CAPACITY-1 elements
	// to keep it as efficient as possible
	// otherwise head==tail when full and empty
	TEST_ASSERT_EQUAL(CB_CAPACITY - 1, circbuf_len(&cb));
}

void test_cb_put_fails_when_full(void) {
	for (int i = 0; i < CB_CAPACITY - 1; i++) {
		TEST_ASSERT_TRUE(circbuf_put(&cb, test_struct));
	}
	TEST_ASSERT_FALSE(circbuf_put(&cb, test_struct));
}

void test_cb_put_overwrite_overwrites_oldest(void) {
	for (int i = 0; i < CB_CAPACITY - 1; i++) {
		TestStruct data = (TestStruct) {i, i};
		circbuf_put_overwrite(&cb, data);
	}
	for (int i = 0; i < CB_CAPACITY - 1; i++) {
		TEST_ASSERT_EQUAL(i, cb.data[i].a);
		TEST_ASSERT_EQUAL(i, cb.data[i].b);
	}
	TEST_ASSERT_TRUE(circbuf_full(&cb));
	TEST_ASSERT_EQUAL(CB_CAPACITY - 1, circbuf_len(&cb));
	// buffer is considered full at CAPACITY-1 so the last entry
	// has yet to be set
	TEST_ASSERT_EQUAL(0, cb.data[CB_CAPACITY - 1].a);
	TEST_ASSERT_EQUAL(0, cb.data[CB_CAPACITY - 1].b);

	circbuf_put_overwrite(&cb, test_struct);
	TEST_ASSERT_EQUAL(1, cb.data[CB_CAPACITY - 1].a);
	TEST_ASSERT_EQUAL(2, cb.data[CB_CAPACITY - 1].b);
	TEST_ASSERT_TRUE(circbuf_full(&cb));
	TEST_ASSERT_EQUAL(CB_CAPACITY - 1, circbuf_len(&cb));

	// this should overwrite oldest entry, at idx 0
	circbuf_put_overwrite(&cb, test_struct2);
	TEST_ASSERT_EQUAL(3, cb.data[0].a);
	TEST_ASSERT_EQUAL(4, cb.data[0].b);
	TEST_ASSERT_TRUE(circbuf_full(&cb));
	TEST_ASSERT_EQUAL(CB_CAPACITY - 1, circbuf_len(&cb));
}

void test_cb_get_fails_when_empty(void) {
	TEST_ASSERT_FALSE(circbuf_get(&cb, &test_struct));
}

void t(void) {
	DECLARE_CIRCBUF(_CircBufPtr, TestStruct *);
	typedef struct _CircBufPtr CircBufPtr;
	CircBufPtr				   cbptr;
	TestStruct				  *buf[16];
	circbuf_init(&cbptr, buf, 16);
	TestStruct *ts = NULL;
	circbuf_put(&cbptr, ts);
	circbuf_get(&cbptr, &ts);
}

DEFINE_CIRCBUF_BUFFER(cb_int, int, 16);

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_cb_init);
	RUN_TEST(test_cb_alloc);
	RUN_TEST(test_cb_put);
	RUN_TEST(test_cb_len);
	RUN_TEST(test_cb_clear);
	RUN_TEST(test_cb_get);
	RUN_TEST(test_cb_empty);
	RUN_TEST(test_cb_full);
	RUN_TEST(test_cb_put_fails_when_full);
	RUN_TEST(test_cb_put_overwrite_overwrites_oldest);
	RUN_TEST(test_cb_get_fails_when_empty);

	return UNITY_END();
}
