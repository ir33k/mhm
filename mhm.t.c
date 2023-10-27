#include "walter.h"
#define MHM_IMPLEMENTATION
#include "mhm.h"

TEST("Basics")
{
	Mhm hm;
	OK(mhm_init(&hm, 100, mhm_hash_djb2) == 0);
	OK(mhm_set(&hm, "aaa", "value0") == 0);
	OK(mhm_set(&hm, "bbb", "value1") == 0);
	OK(mhm_set(&hm, "ccc", "value2") == 0);
	OK(hm._used == 3);
	OK(mhm_has(&hm, "aaa"));
	OK(mhm_has(&hm, "bbb"));
	OK(mhm_has(&hm, "ccc"));
	OK(!mhm_has(&hm, "xxx"));
	SEQ(mhm_get(&hm, "aaa"), "value0");
	SEQ(mhm_get(&hm, "bbb"), "value1");
	SEQ(mhm_get(&hm, "ccc"), "value2");
	mhm_del(&hm, "aaa");
	mhm_del(&hm, "bbb");
	mhm_del(&hm, "ccc");
	OK(hm._used == 0);
	OK(mhm_get(&hm, "aaa") == 0);
	OK(mhm_get(&hm, "bbb") == 0);
	OK(mhm_get(&hm, "ccc") == 0);
	OK(!mhm_has(&hm, "aaa"));
	OK(!mhm_has(&hm, "bbb"));
	OK(!mhm_has(&hm, "ccc"));
	OK(!mhm_has(&hm, "xxx"));
}

TEST("Collisions")
{
	Mhm hm;
	/* Force collisions by using dumb add hashing function and
	 * keys that will produce the same hash value of 100. */
	OK(mhm_init(&hm, 100, mhm_hash_add) == 0);
	OK(mhm_set(&hm, "04", "value0") == 0);
	OK(mhm_set(&hm, "22", "value1") == 0);
	OK(mhm_set(&hm, "d",  "value2") == 0);
	OK(hm._used == 3);
	OK(mhm_has(&hm, "04"));
	OK(mhm_has(&hm, "22"));
	OK(mhm_has(&hm, "d"));
	SEQ(mhm_get(&hm, "04"), "value0");
	SEQ(mhm_get(&hm, "22"), "value1");
	SEQ(mhm_get(&hm, "d"),  "value2");
	mhm_del(&hm, "04");
	mhm_del(&hm, "22");
	mhm_del(&hm, "d");
	OK(hm._used == 0);
	OK(mhm_get(&hm, "04") == 0);
	OK(mhm_get(&hm, "22") == 0);
	OK(mhm_get(&hm, "d")  == 0);
	OK(!mhm_has(&hm, "04"));
	OK(!mhm_has(&hm, "22"));
	OK(!mhm_has(&hm, "d"));
}
