/* My Hash Map

TODO(irek): Implement free() function.
TODO(irek): Better errors.
TODO(irek): Remove items the right way using Lazy deletion [3].
TODO(irek): Define hash function as macro to avoid function calls.

Hash Map implementation using Open addressing [1] with Linear probing
[2] and Lazy deletion [3].  See licence at the end of this file (._. )

[1] https://en.wikipedia.org/wiki/Open_addressing
[2] https://en.wikipedia.org/wiki/Linear_probing
[3] https://en.wikipedia.org/wiki/Lazy_deletion

Characteristics:

- Single header library.
- One malloc() call on initialization.
- You need to know max number/limit of your keys upfront.
- Data storage is responsibility of the user.  Hash Map structure
  stores only pointers to key strings and void pointers to values.
- No dynamic memory management.
- Predefined hash functions but custom can be provided.
- Linear probing in case of collisions.
- Lazy deletion of keys.
- Asserts in main API functions to help avoid major mistakes.

Usage:
	#define MHM_IMPLEMENTATION      // Define once in main file
	#include "mhm.h"

	// Hash Table structure.  Does not require initialization with
	// 0 like "mhm_t ht = {0}" as the mhm_init() will define all
	// structure values anyway.
	mhm_t ht;

	// Initialize Hash Map with 256 slots and djb2 hash function.
	if (mhm_init(&ht, 256, mhm_hash_djb2)) {
		// Handle errors.  The only possible error is fail in
		// malloc() so usually you should be safe but error
		// here is rather critical.
	}

	// Set new value.
	if (mhm_set(&ht, "First", "Your value")) {
		// Yes, this can fail if there are no more empty slots
		// in Hash Map.  If you know upfront that you will not
		// set more keys than available slots then maybe you
		// can skip this error handling.  But don't go crying
		// to me later if you will have hard to debug error
		// because of missing error handling.
		return 1;
	}
	assert(!mhm_set(&ht, "Second", "Test value"))
	assert(!mhm_set(&ht, "Last", "Lorem Ipsum"))

	printf("%s\n", mhm_get(&ht, "First"));  // => "Your value"
	printf("%s\n", mhm_get(&ht, "Second")); // => "Test value"
	printf("%s\n", mhm_get(&ht, "Last"));   // => "Lorem Ipsum"
	assert(0 == mhm_get(&ht, "Unknown"));   // NULL when unknown key

	if (mhm_del(&ht, "First"))              // Deleted
	else                                    // Key was not found.

	if (mhm_has(&ht, "Second"))             // Key was found
	else                                    // Key was not found

	// This is an internal function but useful for debugging.
	// It prints whole memory of hash map to stdout.
	_mhm_dump(&ht);
 */
#ifndef MHM_H
#define MHM_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MHM_NONE SIZE_MAX	/* Used when indexof key was not found */

/* Type of hashing function that takes KEY string and return hash. */
typedef size_t (*Mhm_Hash)(char *key);

typedef struct {
	size_t    _used;        /* Number of used keys */
	size_t    _siz;         /* Size of _key and _buf arrays */
	char    **_key;         /* Buffer with pointers to key strings */
	/* TODO(irek): Make it work with any data type so it is
	 * convenient when you like to us ints or strings with
	 * predefined size itp.  It should be possible to just have
	 * and pointer to a memory and knowledge of single item size
	 * to make it work. */
	void    **_buf;         /* Buffer with pointers to values */
	Mhm_Hash  _hash;        /* Hashing function */
} mhm_t;

size_t mhm_hash_add(char *str);
size_t mhm_hash_djb2(char *str);
int mhm_init(mhm_t *hm, size_t siz, Mhm_Hash hash);
int mhm_set(mhm_t *hm, char *key, void *value);
void *mhm_get(mhm_t *hm, char *key);
int mhm_del(mhm_t *hm, char *key);
int mhm_has(mhm_t *hm, char *key);
size_t _mhm_indexof(mhm_t *hm, char *key);
void _mhm_dump(mhm_t *hm);

/* API reference:
 *
 * mhm_t
 *
 *
 * mhm_hash_add()
 *	Hash STR by adding all bytes.
 *
 * mhm_hash_djb2()
 *	DJB2 (Daniel J. Bernstein) [1][2] hashing function.
 *
 *	[1] http://www.cse.yorku.ca/~oz/hash.html
 *	[2] https://theartincode.stanis.me/008-djb2/
 *
 * mhm_init()
 *	Initialize HM hash map with buffer of SIZ number of slots and HASH
 *	hashing function.  It's expected to use one of mhm_hash_* hash
 *	functions but any custom hashing function can be provided instead.
 *	Return non 0 value on error.  As the only possible error is fail in
 *	call to malloc() you should not expect any failures.
 *
 * mhm_set()
 *	In HM hash map store VALUE pointer to any value under KEY.
 *	Return non 0 value on success.  Set can fail if there is no more
 *	empty slots in hash map.  In that case you should initialize
 *	hash map with more slots upfront.
 *
 * mhm_get()
 *	From HM hash map get KEY item value pointer.
 *
 * mhm_del()
 *	From HM hash map delete KET item.
 *
 * mhm_has()
 *	Return non 0 value if HM hash map has KEY item.
 *
 * _mhm_indexof()
 *	Return index to KEY item in HM hash map.  Return MHM_NONE if not
 *	found.
 *
 * _mhm_dump()
 *	Print whole HM hash map memory buffer to stdout.
 */

#endif /* MHM_H */
#ifdef MHM_IMPLEMENTATION

size_t
mhm_hash_add(char *str)
{
	size_t hash = 0;
	while (*str) {
		hash += *str++;
	}
	return hash;
}

size_t
mhm_hash_djb2(char *str)
{
	size_t hash = 5381;
	while (*str) {
		hash += (hash << 5) + *str++;
	}
	return hash;
}

int
mhm_init(mhm_t *hm, size_t siz, Mhm_Hash hash)
{
	assert(hm);
	assert(siz > 0);
	assert(siz < MHM_NONE);
	assert(hash);
	hm->_used = 0;
	hm->_siz = siz;
	hm->_hash = hash;
	if ((hm->_key = malloc(siz * sizeof(*hm->_key))) == 0) {
		return 1;
	}
	if ((hm->_buf = malloc(siz * sizeof(*hm->_buf))) == 0) {
		return 2;
	}
	memset(hm->_key, 0, siz);
	memset(hm->_buf, 0, siz);
	return 0;
}

int
mhm_set(mhm_t *hm, char *key, void *value)
{
	size_t i;
	assert(hm);
	assert(key);
	if (hm->_used >= hm->_siz) return 1;
	i = (hm->_hash)(key) % hm->_siz;
	while (hm->_key[i]) i = (i + 1) % hm->_siz;
	hm->_key[i] = key;
	hm->_buf[i] = value;
	hm->_used++;
	return 0;
}

void *
mhm_get(mhm_t *hm, char *key)
{
	size_t i;
	assert(hm);
	assert(key);
	i = _mhm_indexof(hm, key);
	if (i == MHM_NONE) return 0;
	return hm->_buf[i];
}

int
mhm_del(mhm_t *hm, char *key)
{
	size_t i;
	assert(hm);
	assert(key);
	i = _mhm_indexof(hm, key);
	if (i == MHM_NONE) {
		return 0;
	}
	hm->_key[i] = 0;
	hm->_buf[i] = 0;
	hm->_used--;
	return 1;
}

int
mhm_has(mhm_t *hm, char *key)
{
	assert(hm);
	assert(key);
	return _mhm_indexof(hm, key) != MHM_NONE;
}

size_t
_mhm_indexof(mhm_t *hm, char *key)
{
	size_t i, j;
	if (hm->_used == 0) {
		return MHM_NONE;
	}
	i = (hm->_hash)(key) % hm->_siz;
	for (j = 0; 1; j++) {
		if (j >= hm->_siz) return MHM_NONE;
		if (hm->_key[i] && *hm->_key[i] == *key && !strcmp(hm->_key[i], key)) {
			break;
		}
		i = (i + 1) % hm->_siz;
	}
	return i;
}

void
_mhm_dump(mhm_t *hm)
{
	size_t i;
	for (i = 0; i < hm->_siz; i++) {
		printf("%s\t%p\n", hm->_key[i], hm->_buf[i]);
	}
	printf("Number of items: %lu\n", hm->_used);
}

#endif /* MHM_IMPLEMENTATION */
/*
This software is available under 2 licenses, choose whichever.
----------------------------------------------------------------------
ALTERNATIVE A - MIT License, Copyright (c) 2024 Irek
Permission is hereby granted, free  of charge, to any person obtaining
a  copy  of this  software  and  associated documentation  files  (the
"Software"), to  deal in  the Software without  restriction, including
without limitation  the rights to  use, copy, modify,  merge, publish,
distribute, sublicense,  and/or sell  copies of  the Software,  and to
permit persons to whom the Software  is furnished to do so, subject to
the  following  conditions:  The   above  copyright  notice  and  this
permission  notice shall  be  included in  all  copies or  substantial
portions of the  Software.  THE SOFTWARE IS PROVIDED  "AS IS", WITHOUT
WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES  OF MERCHANTABILITY,  FITNESS FOR A  PARTICULAR PURPOSE
AND  NONINFRINGEMENT.  IN  NO  EVENT SHALL  THE  AUTHORS OR  COPYRIGHT
HOLDERS BE LIABLE  FOR ANY CLAIM, DAMAGES OR  OTHER LIABILITY, WHETHER
IN AN ACTION  OF CONTRACT, TORT OR OTHERWISE, ARISING  FROM, OUT OF OR
IN CONNECTION  WITH THE SOFTWARE OR  THE USE OR OTHER  DEALINGS IN THE
SOFTWARE.
----------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This  is  free and  unencumbered  software  released into  the  public
domain.  Anyone is free to  copy, modify, publish, use, compile, sell,
or  distribute this  software,  either in  source code  form  or as  a
compiled binary, for any purpose, commercial or non-commercial, and by
any means.  In jurisdictions that recognize copyright laws, the author
or authors of this software dedicate any and all copyright interest in
the software  to the public domain.   We make this dedication  for the
benefit of the public  at large and to the detriment  of our heirs and
successors.   We  intend  this  dedication  to  be  an  overt  act  of
relinquishment in perpetuity of all  present and future rights to this
software  under copyright  law.   THE SOFTWARE  IS  PROVIDED "AS  IS",
WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR IMPLIED, INCLUDING  BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT.  IN NO  EVENT SHALL THE AUTHORS BE LIABLE
FOR ANY  CLAIM, DAMAGES OR  OTHER LIABILITY,  WHETHER IN AN  ACTION OF
CONTRACT, TORT  OR OTHERWISE,  ARISING FROM, OUT  OF OR  IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
