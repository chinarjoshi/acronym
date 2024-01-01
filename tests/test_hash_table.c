#include <stdlib.h>
#include <check.h>
#include "../src/hash_table/hash_table.h"
#include "test.h"

Entry *entry;
HashTable *ht;
const int initial_capacity = 7;
const float load_factor = .5;

static void setup(void) {
    char *command = "git status";
    Status s = create_entry(&entry, command, NULL, NULL, NULL, false);
    if (s == ERR_OUT_OF_MEMORY) {
        ck_abort_msg("Err: out of memory.");
        return;
    }

    Status s2 = create_hash_table(&ht, initial_capacity, load_factor);
    if (s2 == ERR_OUT_OF_MEMORY) {
        ck_abort_msg("Err: out of memory.");
        return;
    }
}

static void teardown(void) {
    // free_hash_table(ht);
}

START_TEST(test_create_hash_table) {
    ck_assert_ptr_nonnull(ht->backing_array);
    ck_assert_int_eq(ht->capacity, initial_capacity);
    ck_assert_int_eq(ht->size, 0);
    ck_assert_float_eq(ht->load_factor, load_factor);
}
END_TEST

START_TEST(test_resize_backing_array) {
    Status s = add_entry(entry, ht);
    ck_assert(s == SUCCESS);
    resize_backing_array(ht);
    ck_assert_int_eq(ht->capacity, initial_capacity * 2 + 1);    
    int hash = hash_alias(entry->alias, ht->capacity);
    ck_assert_ptr_eq(entry, ht->backing_array[hash]);
}
END_TEST

START_TEST(test_add_entry) {
    Status s = add_entry(entry, ht); 
    ck_assert_int_eq(ht->size, 1);
    ck_assert(s == SUCCESS);

    int hash = hash_alias(entry->alias, ht->capacity);
    ck_assert_ptr_eq(ht->backing_array[hash], entry);
}
END_TEST

START_TEST(test_add_entry_trigger_resize) {
    ck_assert_int_eq(ht->capacity, initial_capacity);

    Entry *entries[4];
    create_entry(&entries[0], "cp", NULL, NULL, NULL, false);
    create_entry(&entries[1], "ls -al", NULL, NULL, NULL, false);
    create_entry(&entries[2], "git push -u origin", NULL, NULL, NULL, false);
    entries[3] = entry;

    Status s;
    for (int i = 0; i < 4; i++) {
        s = add_entry(entries[i], ht);
        ck_assert(s == SUCCESS);
        ck_assert_int_eq(ht->size, i + 1);
    }

    for (int i = 0, hash = 0; i < 4; i++) {
        hash = hash_alias(entries[i]->alias, ht->capacity);
        ck_assert_ptr_eq(ht->backing_array[hash], entries[i]);
    }

    ck_assert_int_eq(ht->capacity, initial_capacity * 2 + 1);
}
END_TEST

START_TEST(test_add_entry_duplicate) {
    Status s = add_entry(entry, ht);
    ck_assert(s == SUCCESS);
    Status duplicate_s = add_entry(entry, ht);
    ck_assert(duplicate_s == ERR_DUPLICATE);
}
END_TEST

START_TEST(test_remove_entry) {
    int hash = hash_alias(entry->alias, ht->capacity);
    ht->backing_array[hash] = entry;
    ht->size = 1;

    Entry *data_out;
    Status s = remove_entry(&data_out, entry->alias, ht);
    ck_assert(s == SUCCESS);
    ck_assert_int_eq(ht->size, 0);
    ck_assert(ht->backing_array[hash]->is_removed);
    ck_assert_ptr_eq(data_out, entry);
}
END_TEST

START_TEST(test_remove_entry_empty) {
    Entry *data_out;
    Status s = remove_entry(&data_out, entry->alias, ht);
    ck_assert(s == ERR_NOT_FOUND);
}
END_TEST

START_TEST(test_remove_entry_not_found) {
    int hash = hash_alias(entry->alias, ht->capacity);
    ht->backing_array[hash] = entry;
    ht->size = 1;
    
    Entry *data_out;
    Status s = remove_entry(&data_out, "ls -l", ht);
    ck_assert(s == ERR_NOT_FOUND);
}
END_TEST

START_TEST(test_remove_section) {
    Entry *entries[4];
    create_entry(&entries[0], "git diff", NULL, NULL, NULL, false);
    create_entry(&entries[1], "cp", NULL, NULL, NULL, false);
    create_entry(&entries[2], "git push -u origin", NULL, NULL, NULL, false);
    entries[3] = entry;
    ht->size = 4;

    for (int i = 0; i < 4; i++) {
        ht->backing_array[i] = entries[i];
    }

    Status s = remove_section("git", ht);
    ck_assert(s == SUCCESS);
    ck_assert_int_eq(ht->size, 1);
}
END_TEST

Suite *hash_table_suite(void) {
    Suite *s = suite_create("Hash Table");

    TCase *tc_create = tcase_create("HashTableCreate");
    tcase_add_test(tc_create, test_create_hash_table);
    tcase_add_test(tc_create, test_resize_backing_array);
    tcase_add_checked_fixture(tc_create, setup, teardown);
    suite_add_tcase(s, tc_create);

    TCase *tc_add = tcase_create("HashTableAdd");
    tcase_add_checked_fixture(tc_add, setup, teardown);
    tcase_add_test(tc_add, test_add_entry);
    tcase_add_test(tc_add, test_add_entry_duplicate);
    tcase_add_test(tc_add, test_add_entry_trigger_resize);
    tcase_add_checked_fixture(tc_add, setup, teardown);
    suite_add_tcase(s, tc_add);

    TCase *tc_remove = tcase_create("HashTableRemove");
    tcase_add_checked_fixture(tc_add, setup, teardown);
    tcase_add_test(tc_remove, test_remove_entry);
    tcase_add_test(tc_remove, test_remove_entry_not_found);
    tcase_add_test(tc_remove, test_remove_entry_empty);
    tcase_add_test(tc_remove, test_remove_section);
    tcase_add_checked_fixture(tc_remove, setup, teardown);
    suite_add_tcase(s, tc_remove);

    return s;
}
