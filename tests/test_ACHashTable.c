#include <check.h>
#include "../src/acronym.h"
#include <stdlib.h>

ACEntry *entry;
ACHashTable *ht;
const int initial_capacity = 7;

void setup(void) {
    char *command = "git status";
    Status s = create_ACEntry(&entry, command, NULL, NULL, false);
    if (s == ERR_OUT_OF_MEMORY) {
        return;
    }

    Status s2 = create_ACHashTable(&ht, initial_capacity, .5);
    if (s2 == ERR_OUT_OF_MEMORY) {
        return;
    }
}

void teardown(void) {
    free_ACEntry(entry);
    free_ACHashTable(ht);
}

START_TEST(test_create_ACHashTable) {
    ACHashTable *ht;
    Status s = create_ACHashTable(&ht, 10, .5);
    if (s == ERR_OUT_OF_MEMORY) {
        return;
    }
    ck_assert_ptr_nonnull(ht->backing_array);
    ck_assert_int_eq(ht->capacity, 10);
    ck_assert_int_eq(ht->size, 0);
    ck_assert_float_eq(ht->load_factor, .5);
    free_ACHashTable(ht);
}
END_TEST

START_TEST(test_add_AC) {
    Status s = add_AC(entry, ht); 
    ck_assert_int_eq(ht->size, 1);
    ck_assert(s == SUCCESS);

    int hash = hash_acronym(entry->acronym, ht->capacity);
    ck_assert_ptr_eq(ht->backing_array[hash], entry);
}
END_TEST

START_TEST(test_add_AC_trigger_resize) {
    ck_assert_int_eq(ht->capacity, initial_capacity);

    ACEntry *entries[4];
    create_ACEntry(&entries[0], "cp", NULL, NULL, false);
    create_ACEntry(&entries[1], "ls -al", NULL, NULL, false);
    create_ACEntry(&entries[2], "git push -u origin", NULL, NULL, false);
    entries[3] = entry;

    Status s;
    for (int i = 0; i < 4; i++) {
        s = add_AC(entries[i], ht);
        ck_assert(s == SUCCESS);
        ck_assert_int_eq(ht->size, i + 1);
    }

    ck_assert_int_eq(ht->capacity, initial_capacity * 2 + 1);
}
END_TEST

START_TEST(test_add_AC_duplicate) {
    Status s = add_AC(entry, ht);
    ck_assert(s == SUCCESS);
    Status duplicate_s = add_AC(entry, ht);
    ck_assert(s == ERR_DUPLICATE);
}
END_TEST

START_TEST(test_remove_AC) {
    int hash = hash_acronym(entry->acronym, ht->capacity);
    ht->backing_array[hash] = entry;
    ht->size = 1;

    ACEntry *data_out;
    Status s = remove_AC(&data_out, entry->acronym, ht);
    ck_assert(s == SUCCESS);
    ck_assert_int_eq(ht->size, 0);
    ck_assert(ht->backing_array[hash]->is_removed);
    ck_assert_ptr_eq(data_out, entry);
}
END_TEST

START_TEST(test_remove_AC_empty) {
    ACEntry *data_out;
    Status s = remove_AC(&data_out, entry->acronym, ht);
    ck_assert(s == ERR_NOT_FOUND);
}
END_TEST

START_TEST(test_remove_AC_not_found) {
    int hash = hash_acronym(entry->acronym, ht->capacity);
    ht->backing_array[hash] = entry;
    ht->size = 1;
    
    ACEntry *data_out;
    Status s = remove_AC(&data_out, "ls -l", ht);
    ck_assert(s == ERR_NOT_FOUND);
}
END_TEST

Suite *ACHashTable_suite(void) {
    Suite *s = suite_create("ACHashTable");

    TCase *tc_create = tcase_create("ACHashTableCreate");
    tcase_add_test(tc_create, test_create_ACHashTable);
    suite_add_tcase(s, tc_create);

    TCase *tc_add = tcase_create("ACHashTableAdd");
    tcase_add_checked_fixture(tc_add, setup, teardown);
    tcase_add_test(tc_add, test_add_AC);
    tcase_add_test(tc_add, test_add_AC_duplicate);
    tcase_add_test(tc_add, test_add_AC_trigger_resize);
    suite_add_tcase(s, tc_add);

    TCase *tc_remove = tcase_create("ACHashTableRemove");
    tcase_add_checked_fixture(tc_add, setup, teardown);
    tcase_add_test(tc_add, test_remove_AC);
    tcase_add_test(tc_add, test_remove_AC_not_found);
    tcase_add_test(tc_add, test_remove_AC_empty);
    suite_add_tcase(s, tc_remove);

    return s;
}

 int main(void)
 {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = ACHashTable_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }
