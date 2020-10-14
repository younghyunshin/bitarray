#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "intset.h"

intset *
intset_alloc (int * univ, size_t n_univ) 
{	
	intset * s = (intset *) malloc(sizeof(intset)) ;

	s->univ = univ ;
	s->n_univ = n_univ ;

	size_t bitvect_len = n_univ / 8 + ((n_univ % 8) ? 1 : 0) ;

	s->bitvect = (unsigned char *) malloc(bitvect_len) ;
	memset(s->bitvect, 0, bitvect_len) ;
	s->n_elems = 0 ;
	return s ;
}

intset * 
intset_clone (intset * orig) 
{
	if (orig == 0x0)
		return 0x0 ;

	intset * s ;
	s = intset_alloc(orig->univ, orig->n_univ) ;
	memcpy(s->bitvect, orig->bitvect, orig->n_univ / 8 + ((orig->n_univ % 8) ? 1 : 0)) ;
	s->n_elems = orig->n_elems ;

	return s ;
}

void
intset_print (FILE * fp, intset * s)
{
	fprintf(fp, "{") ;
	char * delim = "" ;
	for (int i = 0 ; i < s->n_univ ; i++) {
		if (intset_contains(s, s->univ[i])) {
			fprintf(fp, "%s%d", delim, s->univ[i]) ;
			delim = ", " ;
		}
	}
	fprintf(fp, "}") ;
}

void
intset_free (intset * s) 
{
	free(s->bitvect) ;
	free(s) ;
}

int
intset_size (intset * s) 
/*
 * returns the number of elements contained in s.
 */
{
	return s->n_elems;

}

int
intset_add (intset * s, int e) 
/*
 * insert a new integer value e to s.
 * return 0 if succeeded. return 1 if it fails.
 * 
 */
{
	 int index = 0;
   int flag = 0;
   for (int i = 0; i < s->n_univ; i++) {
      if (e == s->univ[i]) {
         index = i;
         flag = 1;
         break;
      }
   }
   if (flag == 0) {
      return 1;//e is not in univ
   }
   unsigned char eindex;
   eindex = 128 >> index;
   if ((*(s->bitvect) & eindex) == eindex) {
      return 1; //already e is exist
   }

   //there is no e
   *(s->bitvect) = *(s->bitvect) | eindex;
   s->n_elems = s->n_elems + 1;
   
   //printf("%d",*s->bitvect);
   
   
   return 0;
}

int
intset_remove (intset * s, int e) 
/*
 * remomve e from s.
 * return 0 if succeeded. return 1 if failed.
 *
 */
{
	 if (s->n_elems == 0)return 1;
   int flag = 0;
   int index = 0;
   for (int i = 0; i < s->n_univ; i++) {
      if (e == s->univ[i]) {
         index = i;
         flag = 1;
         break;
      }
   }
   if (flag == 0) {
      return 1;//e is not in univ
   }

   unsigned char eindex;
   eindex = 128 >> index;
   if ((*(s->bitvect) & eindex) == eindex) {
      *(s->bitvect) = *(s->bitvect) ^ eindex;
      s->n_elems = s->n_elems - 1;
      //printf("%d", *s->bitvect);
      return 0; //e is exist
   }
   //there is no e
   //printf("%d", *s->bitvect);
   return 1;
}


int
intset_contains (intset * s, int e) 
/*
 * return 1 if e is contained in s. return 0 otherwise.
 */
{
	if (s->n_elems == 0)return 0;
   int index = 0;
   int flag = 0;
   for (int i = 0; i < s->n_univ; i++) {
      if (e == s->univ[i]) {
         flag = 1;
         index = i;
         break;
      }
   }
   if (flag == 0) {
      return 0; // e is not in univ
   }
   unsigned char eindex;
   eindex = 128 >> index;
   if ((*(s->bitvect) & eindex) == eindex) {
      return 1; //already e is exist
   }

   //there is no e
   return 0;
}


int
intset_equals (intset *s1, intset *s2) 
/*
 * return 1 if two sets s1 and s2 are equivalent.
 * return 0 otherwise.
 *
 * two sets are not equivalent if their univ fields are not the same.
 */
{
	  if (s1 == NULL && s2 == NULL)return 1;
   if (s1 == NULL || s2 == NULL)return 0;
   if (s1->n_elems != s2->n_elems)return 0;//개수 다르면 0
   if (s1->n_univ != s2->n_univ)return 0;
   int count = 0;
   int flag = 0;
   //univ is different -> reuturn 0
   for (int y = 0; y < s1->n_univ; y++) {
      for (int x = 0; x < s2->n_univ; x++) {
         if (s1->univ[y] == s2->univ[x]) {
            count++;
            if (intset_contains(s1, s1->univ[y]) != intset_contains(s2, s2->univ[x])) {
               flag = 1;//들어있는 값다르다
            }
         }
      }
   }
   if (count != s1->n_univ)return 0;//univ원소 다르다

   if (flag == 1)return 0;
   return 1;

}


intset *
intset_union (intset *s1, intset *s2) 
/*
 * return a new intset object that contains the result of
 * the union of s1 and s2.
 *
 * return NULL if the operation fails.
 * union operation fails if their univ fields are not the same.
 */
{
	if (s1 == NULL || s2 == NULL)return NULL;
   if (s1->n_univ != s2->n_univ)return NULL;
   int count = 0;
   for (int y = 0; y < s1->n_univ; y++) {
      for (int x = 0; x < s2->n_univ; x++) {
         if (s1->univ[y] == s2->univ[x]) {
            count++;
         }
      }
   }
   if (count != s1->n_univ)return NULL;//univ원소 다르다
   intset* s = intset_alloc(s1->univ, s1->n_univ);
   //이제 비교해서 or 연산후 값 넣기
   for (int y = 0; y < s1->n_univ; y++) {
      for (int x = 0; x < s2->n_univ; x++) {
         if (s1->univ[y] == s2->univ[x]) {
            if (!(intset_contains(s1, s1->univ[y])==0 &&  intset_contains(s2, s2->univ[x])==0)) {
               intset_add(s, s1->univ[y]);
            }
         }
      }
   }
   
   
   return s;
}


intset *
intset_intersection (intset *s1, intset *s2) 
/*
 * return a new intset object that contains the result of
 * the intersection of s1 and s2.
 *
 * return NULL if the operation fails.
 * intersection operation fails if their univ fields are not the same.
 */
{
	   if (s1 == NULL || s2 == NULL)return NULL;
   if (s1->n_univ != s2->n_univ)return NULL;
   int count = 0;
   for (int y = 0; y < s1->n_univ; y++) {
      for (int x = 0; x < s2->n_univ; x++) {
         if (s1->univ[y] == s2->univ[x]) {
            count++;
         }
      }
   }
   if (count != s1->n_univ)return NULL;//univ원소 다르다
   intset* s = intset_alloc(s1->univ, s1->n_univ);
   //이제 비교해서 or 연산후 값 넣기
   for (int y = 0; y < s1->n_univ; y++) {
      for (int x = 0; x < s2->n_univ; x++) {
         if (s1->univ[y] == s2->univ[x]) {
            if (intset_contains(s1, s1->univ[y]) == 1 && intset_contains(s2, s2->univ[x]) == 1) {
               intset_add(s, s1->univ[y]);
            }
         }
      }
   }
   return s;

}


intset *
intset_difference (intset *s1, intset *s2) 
/*
 * return a new intset object that contains the result of
 * the set difference of s1 and s2 (i.e., s1 \ s2).
 *
 * return NULL if the operation fails.
 * set difference operation fails if their univ fields are not the same.
 */
{
	 if (s1 == NULL || s2 == NULL)return NULL;
   if (s1->n_univ != s2->n_univ)return NULL;
   int count = 0;
   for (int y = 0; y < s1->n_univ; y++) {
      for (int x = 0; x < s2->n_univ; x++) {
         if (s1->univ[y] == s2->univ[x]) {
            count++;
         }
      }
   }
   if (count != s1->n_univ)return NULL;//univ원소 다르다
   intset* s = intset_alloc(s1->univ, s1->n_univ);
   //이제 비교해서 or 연산후 값 넣기
   for (int y = 0; y < s1->n_univ; y++) {
      for (int x = 0; x < s2->n_univ; x++) {
         if (s1->univ[y] == s2->univ[x]) {
            if (intset_contains(s1, s1->univ[y]) == 1 && intset_contains(s2, s2->univ[x]) == 0) {
               intset_add(s, s1->univ[y]);
            }
         }
      }
   }
   return s;

}

intset**
intset_subsets(intset* s, size_t k, size_t* n_subsets)
{
   if (s == NULL) return NULL;
   if (s->n_elems == k) {
      intset** re = (intset**)malloc(sizeof(intset*));
      re[0] = intset_alloc(s->univ, s->n_univ);
      intset* myself = intset_clone(s);
      re[0] = myself;
      *n_subsets = 1;
      return re;
   }

   if (k == 0) {
      intset** ret = (intset**)malloc(sizeof(intset*));
      *n_subsets=0;
      ret[*n_subsets] = intset_alloc(s->univ, s->n_univ);
      return ret;
   }
   intset** re = (intset**)malloc(sizeof(intset*) * 1000);
   intset** Return = (intset**)malloc(sizeof(intset*) * 1000);

   if (k == 1) {
      int f;
      int in = 0;
      for (f = 0; f < s->n_univ; f++) {
         if (intset_contains(s, s->univ[f]) == 1) {
            re[in] = intset_alloc(s->univ, s->n_univ);
            intset_add(re[in], s->univ[f]);
            in++;
         }
      }
      /*
      for (int o = 0; o < s->n_elems; o++) {
         printf("re[%d]", o);
         intset_print(stderr, re[o]);
      }
      */
      *n_subsets = in;
      return re;
   }
   int i;
   int n = 0;
   int j = 0;
   for (i = 0; i < s->n_univ; i++) {
      j = 0;
      if (intset_contains(s, s->univ[i]) == 1) {
         if (j == 0)*n_subsets = 0;
         j++;
         int e = s->univ[i];
         intset* clo = intset_clone(s);
         intset_remove(clo, e);
         re = (intset_subsets(clo, k - 1, n_subsets));

         if (*n_subsets >= 2) {
            //e랑 더하기
            for (int f = 0; f < *n_subsets; f++) {
               intset_add(re[f], e);
            }


            for (int i = 0; i < *n_subsets; i++) {
               int flag = 0;
               for (int p = 0; p < n; p++) {
                  if (intset_equals(Return[p], re[i]) == 1) {
                     flag = 1;
                     break;
                  }
               }
               if (flag == 0) {
                  Return[n] = re[i];
                  n++;
               }

            }
            /*
            for (int o = 0; o < n; o++) {
            printf("Re[%d]", o);
            intset_print(stderr, Return[o]);
            }
            */
         }
         else {
            //중복제거 필요없음, 공집합에 넣은것
            intset_add(re[i], e);
            *n_subsets = i + 1;

            Return[i] = re[i];
         }
      }
   }

   //return
   if (n > * n_subsets) {
      *n_subsets = n;
   }
   return Return;
}


intset**
intset_powerset(intset* s, size_t* n_subsets)
{
   if (s == NULL)return NULL;

   if (s->n_elems == 0) {

      intset** ret = (intset**)malloc(sizeof(intset*));
      ret[*n_subsets] = intset_alloc(s->univ, s->n_univ);
      *n_subsets = *n_subsets + 1;
      return ret;
   }
   intset** re = (intset**)malloc(sizeof(intset*) * 1000);
   intset** Return = (intset**)malloc(sizeof(intset*) * 1000);


   int i;
   int n = 0;
   int j = 0;
   for (i = 0; i < s->n_univ; i++) {
      j = 0;
      if (intset_contains(s, s->univ[i]) == 1) {
         if (j == 0)*n_subsets = 0;
         j++;
         int e = s->univ[i];
         intset* clo = intset_clone(s);
         intset_remove(clo, e);
         re = intset_powerset(clo, n_subsets);
         /*
         for (int i = 0; i < *n_subsets; i++) {
            printf("re[%d]: ", i);
            intset_print(stderr, re[i]);
            printf("\n");
         }
         */
         intset* myself = intset_clone(s);
         re[*n_subsets] = myself;
         *n_subsets = *n_subsets + 1;
         for (int i = 0; i < *n_subsets; i++) {
            int flag = 0;
            for (int p = 0; p < n; p++) {
               if (intset_equals(Return[p], re[i]) == 1) {
                  flag = 1;
                  break;
               }
            }
            if (flag == 0) {
               Return[n] = re[i];
               n++;
            }

         }
         /*
         for (int o = 0; o < n; o++) {
            printf("Re[%d]", o);
            intset_print(stderr, Return[o]);
         }
         */
      }
   }

   //return
   if (n > * n_subsets) {
      *n_subsets = n;
   }


   return Return;
}
