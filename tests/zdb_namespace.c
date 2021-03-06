#include <stdio.h>
#include <string.h>
#include "tests_user.h"
#include "zdb_utils.h"
#include "tests.h"

// sequential priority
#define sp 160

static char *namespace_default = "default";
static char *namespace_created = "test_ns_create";
static char *namespace_protected = "test_ns_protected";
static char *namespace_password = "helloworld";
static char *namespace_password_try1 = "blabla";
static char *namespace_password_try2 = "hellowo";
static char *namespace_password_try3 = "helloworldhello";
static char *namespace_maxsize = "test_ns_maxsize";

// select not existing namespace
runtest_prio(sp, namespace_select_not_existing) {
    const char *argv[] = {"SELECT", "notfound"};
    return zdb_command_error(test, argvsz(argv), argv);
}



// create a new namespace
runtest_prio(sp, namespace_create) {
    return zdb_nsnew(test, namespace_created);
}

// select this new namespace
runtest_prio(sp, namespace_select_created) {
    const char *argv[] = {"SELECT", namespace_created};
    return zdb_command(test, argvsz(argv), argv);
}

// do a set on this namespace
runtest_prio(sp, namespace_simple_set) {
    return zdb_set(test, "hello", "world");
}

// read the value back
runtest_prio(sp, namespace_simple_get) {
    return zdb_check(test, "hello", "world");
}

// write a new value on this namespace
runtest_prio(sp, namespace_special_set) {
    return zdb_set(test, "special-key", "hello");
}

// read this new value to be sure
runtest_prio(sp, namespace_special_get) {
    return zdb_check(test, "special-key", "hello");
}



// move back to default namespace
runtest_prio(sp, namespace_switchback_default) {
    const char *argv[] = {"SELECT", namespace_default};
    return zdb_command(test, argvsz(argv), argv);
}

// we should not find "special-key" here (another namespace)
runtest_prio(sp, namespace_default_ensure) {
    const char *argv[] = {"GET", "special-key"};
    return zdb_command_error(test, argvsz(argv), argv);
}



// create a new namespace
runtest_prio(sp, namespace_create_protected) {
    return zdb_nsnew(test, namespace_protected);
}

// set password on this namespace
runtest_prio(sp, namespace_set_password) {
    const char *argv[] = {"NSSET", namespace_protected, "password", namespace_password};
    return zdb_command(test, argvsz(argv), argv);
}

// set it as private
runtest_prio(sp, namespace_set_protected) {
    const char *argv[] = {"NSSET", namespace_protected, "public", "0"};
    return zdb_command(test, argvsz(argv), argv);
}



// try to select it with a wrong password
runtest_prio(sp, namespace_select_protected_pass_try1) {
    const char *argv[] = {"SELECT", namespace_protected, namespace_password_try1};
    return zdb_command_error(test, argvsz(argv), argv);
}

// try to select it with a correct prefix-password (see #21)
runtest_prio(sp, namespace_select_protected_pass_try2) {
    const char *argv[] = {"SELECT", namespace_protected, namespace_password_try2};
    return zdb_command_error(test, argvsz(argv), argv);
}

// try to select it with a longer prefix-correct password (see #21)
runtest_prio(sp, namespace_select_protected_pass_try3) {
    const char *argv[] = {"SELECT", namespace_protected, namespace_password_try3};
    return zdb_command_error(test, argvsz(argv), argv);
}

// try to select it without password
runtest_prio(sp, namespace_select_protected_nopass) {
    const char *argv[] = {"SELECT", namespace_protected};
    return zdb_command_error(test, argvsz(argv), argv);
}

// try to select it with the right password
runtest_prio(sp, namespace_select_protected_correct_pass) {
    const char *argv[] = {"SELECT", namespace_protected, namespace_password};
    return zdb_command(test, argvsz(argv), argv);
}



// go back to default, again
runtest_prio(sp, namespace_switchback_default_2) {
    const char *argv[] = {"SELECT", namespace_default};
    return zdb_command(test, argvsz(argv), argv);
}

// try to switch to the same namespace we currently are
runtest_prio(sp, namespace_switchback_default_again) {
    const char *argv[] = {"SELECT", namespace_default};
    return zdb_command(test, argvsz(argv), argv);
}



// moving protected to unprotected now
runtest_prio(sp, namespace_set_public) {
    const char *argv[] = {"NSSET", namespace_protected, "public", "1"};
    return zdb_command(test, argvsz(argv), argv);
}

// try to select it without password
runtest_prio(sp, namespace_select_public) {
    const char *argv[] = {"SELECT", namespace_protected};
    return zdb_command(test, argvsz(argv), argv);
}

// we should be in read-only now
runtest_prio(sp, namespace_write_on_protected) {
    const char *argv[] = {"SET", "should", "fails"};
    return zdb_command_error(test, argvsz(argv), argv);
}



// try to change settings on 'default'
runtest_prio(sp, namespace_set_default_private) {
    const char *argv[] = {"NSSET", namespace_default, "public", "0"};
    return zdb_command_error(test, argvsz(argv), argv);
}

runtest_prio(sp, namespace_set_default_sizelimit) {
    const char *argv[] = {"NSSET", namespace_default, "maxsize", "42"};
    return zdb_command_error(test, argvsz(argv), argv);
}

runtest_prio(sp, namespace_set_default_password) {
    const char *argv[] = {"NSSET", namespace_default, "password", "hello"};
    return zdb_command_error(test, argvsz(argv), argv);
}



// check maxsize effect
runtest_prio(sp, namespace_create_maxsize) {
    return zdb_nsnew(test, namespace_maxsize);
}

runtest_prio(sp, namespace_set_limit) {
    const char *argv[] = {"NSSET", namespace_maxsize, "maxsize", "16"};
    return zdb_command(test, argvsz(argv), argv);
}

runtest_prio(sp, namespace_select_maxsize) {
    const char *argv[] = {"SELECT", namespace_maxsize};
    return zdb_command(test, argvsz(argv), argv);
}

// write 10 bytes
runtest_prio(sp, namespace_limit_write1) {
    return zdb_set(test, "key1", "0123456789");
}

// write 5 bytes
runtest_prio(sp, namespace_limit_write2) {
    return zdb_set(test, "key2", "abcde");
}

// write 1 byte (exact limit, 10 + 5 + 1 = 16)
runtest_prio(sp, namespace_limit_write_exact_limit) {
    return zdb_set(test, "key3", "+");
}

// writing 1 more byte should fail
runtest_prio(sp, namespace_limit_write_over) {
    const char *argv[] = {"SET", "key4", "X"};
    return zdb_command_error(test, argvsz(argv), argv);
}

// replacing 1 byte key by 1 another byte is allowed
runtest_prio(sp, namespace_limit_replace_exact) {
    return zdb_set(test, "key3", "-");
}

// replacing key1 which was 10 bytes, by 5 bytes
runtest_prio(sp, namespace_limit_replace_shrink) {
    return zdb_set(test, "key1", "12345");
}

// try to write over again
runtest_prio(sp, namespace_limit_write_over_shrink) {
    const char *argv[] = {"SET", "key5", "67890X"};
    return zdb_command_error(test, argvsz(argv), argv);
}

// try to write exact size again
runtest_prio(sp, namespace_limit_write_in_shrink) {
    return zdb_set(test, "key5", "67890");
}

