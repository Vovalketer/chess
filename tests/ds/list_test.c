#include "list.h"

#include "../../external/unity/unity.h"

struct test_struct {
	int				 a;
	int				 b;
	struct list_node node;
};

struct list_node   head;
struct test_struct gts;
struct test_struct gts2;
struct test_struct gts3;

void setUp(void) {
	head = (struct list_node) {.next = &head, .prev = &head};
	gts	 = (struct test_struct) {1, 1, .node.next = &gts.node, .node.prev = &gts.node};
	gts2 = (struct test_struct) {2, 2, .node.next = &gts2.node, .node.prev = &gts2.node};
	gts3 = (struct test_struct) {3, 3, .node.next = &gts3.node, .node.prev = &gts3.node};
}

void tearDown(void) {
}

void test_list_head(void) {
	LIST_HEAD(list);
	TEST_ASSERT_EQUAL(&list, list.next);
	TEST_ASSERT_EQUAL(&list, list.prev);
}

void test_list_init(void) {
	TEST_ASSERT_EQUAL(&head, head.next);
	TEST_ASSERT_EQUAL(&head, head.prev);
}

void test_list_add(void) {
	list_add(&gts.node, &head);
	TEST_ASSERT_EQUAL(head.next, &gts.node);
	TEST_ASSERT_EQUAL(head.prev, &gts.node);
}

void test_list_add_three(void) {
	list_add(&gts.node, &head);
	list_add(&gts2.node, &head);
	list_add(&gts3.node, &head);
	TEST_ASSERT_EQUAL(head.next, &gts3.node);
	TEST_ASSERT_EQUAL(head.prev, &gts.node);
}

void test_list_add_tail(void) {
	list_add_tail(&gts.node, &head);
	TEST_ASSERT_EQUAL(head.next, &gts.node);
	TEST_ASSERT_EQUAL(head.prev, &gts.node);
}

void test_list_add_tail_three(void) {
	list_add_tail(&gts.node, &head);
	list_add_tail(&gts2.node, &head);
	list_add_tail(&gts3.node, &head);
	TEST_ASSERT_EQUAL(head.next, &gts.node);
	TEST_ASSERT_EQUAL(head.prev, &gts3.node);
}

void test_list_del(void) {
	list_add(&gts.node, &head);
	list_del(&gts.node);
	TEST_ASSERT_EQUAL(&head, head.next);
	TEST_ASSERT_EQUAL(&head, head.prev);
}

void test_list_del_middle(void) {
	list_add(&gts.node, &head);
	list_add(&gts2.node, &head);
	list_add(&gts3.node, &head);
	TEST_ASSERT_EQUAL(gts3.node.next, &gts2.node);
	TEST_ASSERT_EQUAL(gts.node.prev, &gts2.node);

	list_del(&gts2.node);
	TEST_ASSERT_EQUAL(gts3.node.next, &gts.node);
	TEST_ASSERT_EQUAL(gts.node.prev, &gts3.node);
}

void test_list_first_entry(void) {
	list_add(&gts.node, &head);
	struct test_struct *first = list_first_entry(&head, struct test_struct, node);

	TEST_ASSERT_EQUAL(&gts, first);
}

void test_list_for_each_entry(void) {
	list_add(&gts.node, &head);
	list_add(&gts2.node, &head);
	list_add(&gts3.node, &head);
	struct test_struct *current;
	int					i = 3;
	list_for_each_entry(current, &head, node) {
		TEST_ASSERT_EQUAL(i--, current->a);
	}
}

void test_list_for_each_entry_reverse(void) {
	list_add(&gts.node, &head);
	list_add(&gts2.node, &head);
	list_add(&gts3.node, &head);
	struct test_struct *current;
	int					i = 1;
	list_for_each_entry_reverse(current, &head, node) {
		TEST_ASSERT_EQUAL(i++, current->a);
	}
}

void test_list_last_entry(void) {
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_list_head);
	RUN_TEST(test_list_init);
	RUN_TEST(test_list_add);
	RUN_TEST(test_list_add_three);
	RUN_TEST(test_list_add_tail);
	RUN_TEST(test_list_add_tail_three);
	RUN_TEST(test_list_del);
	RUN_TEST(test_list_del_middle);
	RUN_TEST(test_list_first_entry);
	RUN_TEST(test_list_for_each_entry);
	RUN_TEST(test_list_for_each_entry_reverse);
	return UNITY_END();
}
