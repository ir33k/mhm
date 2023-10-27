/**
 * My Hash Map
 *
 * Single header library.
 */

#ifndef MHM_H
#define MHM_H

#include <assert.h>
#include <stdint.h>
#include <string.h>

#define MHM_NONE SIZE_MAX       /* Used when indexof key was not found */

/* Type of hashing function that takes KEY string and return hash. */
typedef size_t (*Mhm_Hash)(char *key);

typedef struct mhm {            /* Hash Map structure */
	size_t    _used;        /* Number of used keys */
	size_t    _siz;         /* Size of _key and _buf arrays */
	char    **_key;         /* Buffer with pointers to key strings */
	void    **_buf;         /* Buffer with pointers to values */
	Mhm_Hash  _hash;        /* Hashing function */
} Mhm;

/* Initialize HM hash map with buffer of SIZ size and HASH hashing
 * function.  It's expected to use one of mhm_hash_* hash functions
 * but any custom hashing function can be provided instead.  Return
 * non 0 value on error. */
int mhm_init(Mhm *hm, size_t siz, Mhm_Hash hash);

/* In HS hash map store VALUE pointer to any value under KEY.  Return
 * non 0 value on success. */
int mhm_set(Mhm *hm, char *key, void *value);

/* From HS hash map get KEY item value pointer. */
void *mhm_get(Mhm *hm, char *key);

/* From HS hash map delete KET item*/
void mhm_del(Mhm *hm, char *key);

/* Return non 0 value if HM hash map has KEY item. */
int mhm_has(Mhm *hm, char *key);

/* The simplest possible hashing function that adds all bytes of STR
 * string to create returned hash value. */
size_t mhm_hash_add(char *str);

/* Takes STR key string and returns DJB2 hash. */
size_t mhm_hash_djb2(char *str);

/* Return index to KEY item in HM hash map.  Return MHM_NONE if not
 * found. */
size_t _mhm_indexof(Mhm *hm, char *key);

/* Print whole HM hash map memory buffer to stdout. */
void _mhm_dump(Mhm *hm);

#endif /* MHM_H */
#ifdef MHM_IMPLEMENTATION

int
mhm_init(Mhm *hm, size_t siz, Mhm_Hash hash)
{
	assert(hm);
	assert(siz > 0);
	assert(siz < MHM_NONE);
	assert(hash);
	hm->_used = 0;
	hm->_siz = siz;
	if ((hm->_key = malloc(siz * sizeof(*hm->_key))) == 0) return 1;
	if ((hm->_buf = malloc(siz * sizeof(*hm->_buf))) == 0) return 2;
	memset(hm->_key, 0, siz);
	memset(hm->_buf, 0, siz);
	hm->_hash = hash;
	return 0;
}

int
mhm_set(Mhm *hm, char *key, void *value)
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
mhm_get(Mhm *hm, char *key)
{
	size_t i;
	assert(hm);
	assert(key);
	i = _mhm_indexof(hm, key);
	if (i == MHM_NONE) return 0;
	return hm->_buf[i];
}

void
mhm_del(Mhm *hm, char *key)
{
	size_t i;
	assert(hm);
	assert(key);
	i = _mhm_indexof(hm, key);
	if (i == MHM_NONE) return;
	hm->_key[i] = 0;
	hm->_buf[i] = 0;
	hm->_used--;
}

int
mhm_has(Mhm *hm, char *key)
{
	size_t i;
	assert(hm);
	assert(key);
	i = _mhm_indexof(hm, key);
	if (i == MHM_NONE) return 0;
	return 1;
}

size_t
mhm_hash_add(char *str)
{
	size_t hash = 0;
	while (*str) hash += *str++;
	return hash;
}

/* Modified version of https://theartincode.stanis.me/008-djb2/ */
size_t
mhm_hash_djb2(char *str)
{
	size_t hash = 5381;
	while (*str) hash += (hash << 5) + *str++;
	return hash;
}

size_t
_mhm_indexof(Mhm *hm, char *key)
{
	size_t i, j;
	if (hm->_used == 0) return MHM_NONE;
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
_mhm_dump(Mhm *hm)
{
	size_t i;
	for (i = 0; i < hm->_siz; i++) {
		printf("%lu\t%p\n", i, hm->_buf[i]);
	}
	printf("Number of items: %lu\n", hm->_used);
}

#endif /* MHM_IMPLEMENTATION */
