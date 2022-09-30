#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// Define the max element length
const int N = 101;

struct node
{
    // Define the numerical semigroup element
    int element;

    struct node *next;
};

// Define a node for each element in dec_k (M) - the set of coefficients for each maximal decomposition of length k.
struct dec_node
{
    // Define an array to hold the coefficients for each monoid element
    int coef[6];

    struct dec_node *next;
};

struct dec_node *create_dec_nodes(struct dec_node *n0, struct dec_node *n1, int *arr0, struct node **arr1, int E, int k);
struct node *create_monoid(struct node *n0, int x, int *arr0, int E);
bool compare(int *x, struct node *n, int E);
void print_MONOID(struct node **n, int M_MAX);
void print_dec_node_list(struct dec_node *n, int E);
void free_MONOID(struct node **n, int M_MAX);
void free_dec(struct dec_node **n);
void print_dec_node(const char *label, struct dec_node *n, int E);
void duplicates(struct dec_node *ptr, int E);
struct dec_node *remove_non_max(struct dec_node *n0, struct dec_node *n1, int *arr0, struct node **arr1, int E, int k);
bool find(int z, struct node *ptr, int *arr, int E);
int gamma(struct dec_node *n, int *arr, int E);
int size(struct dec_node *ptr);
int size_hash(struct node *ptr);
struct node *hash_dec_nodes(struct dec_node *n, int *arr0, int E);
void duplicates_hash(struct node *ptr, int E);
void free_hash(struct node **n);
void bubble_sort(int *arr, int n);
int gcd(int a, int b);
int findGCD(int *arr, int n);
void print_gen_set(int *arr, int E);
void create_gen_set(int *arr, int max, int min, int E);
bool gen_duplicate(int x, int *arr, int end);

int main(void)
{
	// Start programming timming
    clock_t start, end;
	double cpu_time_used;
	start = clock();

	// Declare the embedding dimension variable E
	int i, j, k, l, E, index, min, max, num_range;

	num_range = 1;

	FILE *file = fopen("intervals.csv", "r");
	if (file == NULL) {
		printf("Could not open file, line %d in function %s.\n", __LINE__, __func__);
		return 1;
	}
	int arr0[num_range][2];
	memset(arr0, 0, sizeof(arr0));
	char line[30];
	char *ptr;
	index = 0;
	while (fgets(line, 30, file) != NULL) {
		ptr = strtok(line, ",");
		arr0[index][0] = atoi(ptr);
		ptr = strtok(NULL, ",");
		arr0[index][1] = atoi(ptr);
		index++;
	}
	fclose(file);

	for (i = 0; i < num_range; i++){
		printf("%i, %i\n", arr0[i][0], arr0[i][1]);
	}

	// Define array that holds each number for the embedding dimensions, i.e. the number of
	// generators to produce.
	int arr1[] = {3};
	int size0 = sizeof(arr1) / sizeof(int);

	for (k = 0; k < num_range; k++)
	{
		min = arr0[k][0];
		max = arr0[k][1];

		for (l = 0; l < size0; l++)
		{
			E = arr1[l];
			int gen_set[E];
			create_gen_set(gen_set, max, min, E);
			printf("Currently working on range %i to %i.\n", min, max);
			print_gen_set(gen_set, E);
			// Define the max element in the numerical monoid
			const int M_MAX = (N*gen_set[E - 1]) - gen_set[0];

			// Declare an array of pointers to monoid elements in struct node and fill with NULL
			struct node *MONOID[M_MAX];
			for (i = 0; i < M_MAX; i++)
				MONOID[i] = NULL;

			// Inductively create the monoid
			for (i = 0; i < M_MAX; i++) {
				if (i == 0){
					MONOID[0] = malloc(sizeof(struct node));
					if (!MONOID[0]) {
						printf("ERROR: line number %d in function %s\n", __LINE__, __func__);
					}
					memset(MONOID[0], 0, sizeof(*MONOID[0]));
				}
				else {
					MONOID[i] = create_monoid(MONOID[i - 1], i, gen_set, E);
				}
			}

			// Declare an array of pointers to a dec_node linked list and fill with NULL
			struct dec_node *dec[N];
			for (i = 0; i < N; i++)
				dec[i] = NULL;

			// Build dec_k(M)
			for (i = 0; i < N; i++) {
				if (i == 0)
					dec[0] = create_dec_nodes(NULL, NULL, NULL, NULL, E, 0);
				else if (i == 1)
					dec[1] = create_dec_nodes(dec[0], NULL, NULL, NULL, E, 1);
				else
					dec[i] = create_dec_nodes(dec[1], dec[i-1], gen_set, MONOID, E, i);
			}

			// Remove duplicate elements with equal lengths
			struct node *hash_table[N];
			for (i = 0; i < N; i++)
				hash_table[i] = NULL;
			for (i = 0; i < N; i++)
				hash_table[i] = hash_dec_nodes(dec[i], gen_set, E);

			char buffer0[50];
			sprintf(buffer0, "output/%i_%i_%i", min, max, E);
			char buffer1[5];
			for (i = 0; i < E; i++) {
				sprintf(buffer1, "_%i", gen_set[i]);
				strncat(buffer0, buffer1, 5);
			}
			sprintf(buffer1, ".csv");
			strncat(buffer0, buffer1, 5);

			file = fopen(buffer0, "w");
		    if (file == NULL){
		        printf("Could not open file, line %d in function %s.\n", __LINE__, __func__);
		        return 1;
		    }
		    fprintf(file, "k, d_k(M)\n");
		    for (i = 0; i < N; i++)
		    	fprintf(file, "%i, %i\n", i, size_hash(hash_table[i]));
		    fclose(file);

			free_MONOID(MONOID, M_MAX);
			free_dec(dec);
			free_hash(hash_table);
		}
	}
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Program took %f seconds to execute\n", cpu_time_used);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC / 60;
    printf("Program took %f minutes to execute\n", cpu_time_used);
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC / 60 / 60;
    printf("Program took %f hours to execute\n", cpu_time_used);
    return 0;
}

bool gen_duplicate(int x, int *arr, int end)
{
	for (int i = 0; i < end; i++) {
		if (arr[i] == x) {
			return true;
		}
	}
	return false;
}

void create_gen_set(int *arr, int max, int min, int E)
{
	int i;
	// Fill gen_set with random integers between min and max
	srand(time(NULL));

	for (i = 0; i < E; i++) {
		arr[i] = (rand() % (max - min)) + min;
		if (i > 0 && gen_duplicate(arr[i], arr, i)) {
			i--;
		}
	}

	// Sort gen_set from minimum value to maximum for readability
	bubble_sort(arr, E);

	// Check with the elements in gen_set are coprime.
	// If they are not, divide all elements by the gcd to get a coprime list.
	int gcd = findGCD(arr, E);
	if (gcd != 1){
		for (i = 0; i < E; i++){
			arr[i] = arr[i] / gcd;
		}
	}
}

void print_gen_set(int *arr, int E)
{
	printf("The generators are: ");
	for (int i = 0; i < E; i ++){
		printf("%i", arr[i]);
		if (i < E - 1){
			printf(", ");
		}
	}
	printf("\n");
}


struct dec_node *create_dec_nodes(struct dec_node *n0, struct dec_node *n1, int *arr0, struct node **arr1, int E, int k)
{
	int i, j;
	struct dec_node *n;
	struct dec_node *list = NULL;
	struct dec_node *tmp1;

	if (n0 == NULL){
		list = malloc(sizeof(*list));
		if (!list) {
			printf("ERROR: line number %d in function %s\n", __LINE__, __func__);
		}
		memset(list, 0, sizeof(*list));
	}
	else if (n1 == NULL){
		for (i = 0; i < E; i++){ // Put the right number of zeros at the front of the vector
			n = malloc(sizeof(*n));
			if (!n) {
				printf("ERROR: line number %d in function %s\n", __LINE__, __func__);
			}
			for (j = 0; j < i; j++)
				n -> coef[j] = 0;
			n -> coef[j] = 1; // Put 1 in the right place
			for (j = j + 1; j < E; j++) // Fill in the rest of the zeros
				n -> coef[j] = 0;
			n -> next = list;
			list = n;
		}
	}
	else {
		while (n0 != NULL){
			tmp1 = n1;
			while (tmp1 != NULL){
				n = malloc(sizeof(*n));
				if (!n) {
					printf("ERROR: line number %d in function %s\n", __LINE__, __func__);
				}
				for (i = 0; i < E; i++)
					n -> coef[i] = n0 -> coef[i] + tmp1 -> coef[i];
				n -> next = list;
				list = n;
				tmp1 = tmp1 -> next;
			}
			n0 = n0 -> next;
		}
		if (list != NULL) {
			duplicates(list, E);
			list = remove_non_max(list, n1, arr0, arr1, E, k);
		}
	}
	return list;
}

struct dec_node *remove_non_max(struct dec_node *n0, struct dec_node *n1, int *arr0, struct node **arr1, int E, int k)
{
	// arr0 = gen_set
	// arr1 = MONOID
	// n0 = dec[i] = list
	// n1 = dec[i-1]

	struct dec_node *tmp0, *tmp1, *prev;
	int M = (k+1)*arr0[E - 1] - k*arr0[0];
	tmp0 = n0;
	tmp1 = n1;

	// If n0 node itself holds the key to be deleted
	while (tmp1 != NULL && tmp0 != NULL)
	{
		if (find(gamma(tmp0, arr0, E) - gamma(tmp1, arr0, E), arr1[M+1], arr0, E))
		{
			n0 = tmp0 -> next;
			free(tmp0);
			tmp0 = n0;
			tmp1 = n1;
		}
		else {
			tmp1 = tmp1 -> next;
		}
	}

	// Advance tmp0 since the first node is checked
	tmp0 = n0 -> next;
	prev = n0;

	while (tmp0 != NULL)
	{
		tmp1 = n1;
		while (tmp1 != NULL)
		{
			if (find(gamma(tmp0, arr0, E) - gamma(tmp1, arr0, E), arr1[M+1], arr0, E))
			{
				prev -> next = tmp0 -> next;
				free(tmp0);
				tmp0 = prev -> next;
				tmp1 = n1;
			}
			else {
				tmp1 = tmp1 -> next;
			}
		}
		if (tmp0 != NULL)
		{
			tmp0 = tmp0 -> next;
			prev = prev -> next;
		}
	}
	return n0;
}

struct node *hash_dec_nodes(struct dec_node *n0, int *arr0, int E)
{
	struct dec_node *tmp = n0;
	struct node *list = NULL;
	struct node *n = NULL;
	while (tmp != NULL)
	{
		n = malloc(sizeof(*n));
		if (!n) {
			printf("ERROR: line number %d in function %s\n", __LINE__, __func__);
		}
		n -> element = gamma(tmp, arr0, E);
		n -> next = list;
		list = n;
		tmp = tmp -> next;
	}
	duplicates_hash(list, E);
	return list;
}



void duplicates_hash(struct node *list, int E)
{
	struct node *tmp0, *tmp1, *prev;
	int i, j;
	tmp0 = list;
	while (tmp0 != NULL && tmp0 -> next != NULL)
	{
		prev = tmp0;
		tmp1 = prev -> next;

		while (tmp1 != NULL)
		{
			if (tmp0 -> element != tmp1 -> element) {
				prev = tmp1;
				tmp1 = prev -> next;
			}
			else if (tmp0 -> element == tmp1 -> element) {
				prev -> next = tmp1 -> next;
				free(tmp1);
				tmp1 = prev -> next;
			}
		}
		tmp0 = tmp0 -> next;
	}
}


int gamma(struct dec_node *n, int *arr, int E)
{
	int x = 0;
	for (int i = 0; i < E && n != NULL; i++) {
		x += n -> coef[i] * arr[i];
	}
	return x;
}

bool find(int z, struct node *ptr, int *arr, int E)
{
	// Check to see if z is an elemnet in the Hilbert basis. If so, then return false.
	for (int i = 0; i < E; i++) {
		if (z == arr[i])
			return false;
	}
	struct node *tmp_1 = ptr;
	while (tmp_1 != NULL) {
		if (z == tmp_1 -> element) {
			return true;
		}
		tmp_1 = tmp_1 -> next;
	}
	return false;
}

void print_dec_node_list(struct dec_node *n, int E)
{
	// Print out the elements in the i'th dec set
	while (n != NULL) {
		printf("(%i", n -> coef[0]);
		for (int i = 1; i < E; i++)
			printf(", %i", n -> coef[i]);
		printf(")  ");
		n = n -> next;
	}
	printf("\n");
}

void print_dec_node(const char *label, struct dec_node *n, int E)
{
	printf("%s: ", label);
	printf("(%i", n -> coef[0]);
	for (int i = 1; i < E; i++)
		printf(", %i", n -> coef[i]);
	printf(")\n");
}

// MONOID[i] = create_monoid(MONOID[i - 1], i, gen_set, E);
struct node *create_monoid(struct node *n0, int k, int *arr0, int E)
{
	int i;
	int arr1[E];
	struct node *tmp0 = n0;
	struct node *list = NULL;
	struct node *n;

	// Calculate the set k - Hilb(M)
	for (i = 0; i < E; i++)
	{
		// *(arr1 + i) = k - *(arr0 + i);
		arr1[i] = k - arr0[i];
	}

	while (tmp0 != NULL)
	{
		n = malloc(sizeof(*n));
		if (!n) {
			printf("ERROR: line number %d in function %s\n", __LINE__, __func__);
		}
		n -> element = tmp0 -> element;
		n -> next = list;
		list = n;
		tmp0 = tmp0 -> next;
	}
	if (compare(arr1, n0, E))
	{
		n = malloc(sizeof(*n));
		if (!n) {
			printf("ERROR: line number %d in function %s\n", __LINE__, __func__);
		}
		n -> element = k;
		n -> next = list;
		list = n;
	}
	return list;
}

bool compare(int *x, struct node *n, int E)
{
	int i;
	while (n != NULL)
	{
		for (i = 0; i < E; i++)
		{
			if (*(x + i) == (*n).element)
			{
				return true;
			}
			/* x[i] == n -> element; */
		}
		n = (*n).next;
	}
	return false;
}

void free_MONOID(struct node **n, int M_MAX)
{
	struct node *tmp1, *tmp2;

	for (int i = 0; i < M_MAX; i++)
	{
		tmp1 = *(n + i);

		while (tmp1 != NULL)
		{
			tmp2 = tmp1 -> next;
			free(tmp1);
			tmp1 = tmp2;
		}

	}
}

void free_dec(struct dec_node **n)
{
	struct dec_node *tmp1, *tmp2;

	for (int i = 0; i < N; i++)
	{
		tmp1 = n[i];
		while (tmp1 != NULL)
		{
			tmp2 = tmp1 -> next;
			free (tmp1);
			tmp1 = tmp2;
		}
	}
}

void free_hash(struct node **n)
{
	struct node *tmp1, *tmp2;

	for (int i = 0; i < N; i++)
	{
		tmp1 = n[i];
		while (tmp1 != NULL)
		{
			tmp2 = tmp1 -> next;
			free (tmp1);
			tmp1 = tmp2;
		}
	}
}

void print_MONOID(struct node **n, int M_MAX)
{
	struct node *tmp0;

	for (int i = 0; i < M_MAX; i++)
	{
		tmp0 = *(n + i);
		while (tmp0 != NULL)
		{
			printf("%i ", tmp0 -> element);
			tmp0 = tmp0 -> next;
		}
		printf("\n");
	}

}

void duplicates(struct dec_node *ptr, int E)
{
	struct dec_node *tmp0, *tmp1, *prev;

	int i, j;
	tmp0 = prev = ptr;
	while (tmp0 -> next != NULL)
	{
		prev = tmp0;
		tmp1 = tmp0 -> next;

		while (tmp1 != NULL)
		{
			//print_nodes("compare", tmp0, tmp1);
			for (j = 0; j < E; j++) {
				 if (tmp0 -> coef[j] != tmp1 -> coef[j]) break;
			}
			if (j == E) {
				/* duplicate */
				prev->next = tmp1->next;
				assert(tmp1 != ptr);
				free(tmp1);
				tmp1 = prev->next;
			}
			else {
				prev = tmp1;
				tmp1 = tmp1->next;
			}

		}
		tmp0 = tmp0 -> next;
	}
}

int size(struct dec_node *ptr)
{
    int counter = 0;
    struct dec_node *tmp = ptr;
    while (tmp != NULL)
    {
        counter++;
        tmp = tmp -> next;
    }
    return counter;
}

int size_hash(struct node *ptr)
{
    int counter = 0;
    struct node *tmp = ptr;
    while (tmp != NULL)
    {
        counter++;
        tmp = tmp -> next;
    }
    return counter;
}

void bubble_sort(int *arr, int n) {
    int i = 0, j = 0, tmp;
    for (i = 0; i < n; i++) {   // loop n times - 1 per element
        for (j = 0; j < n - i - 1; j++) { // last i elements are sorted already
            if (arr[j] > arr[j + 1]) {  // swop if order is broken
                tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}


// Function to return gcd of a and b
int gcd(int a, int b)
{
	if (a == 0)
		return b;
	return gcd(b % a, a);
}

// Function to find gcd of array of
// numbers
int findGCD(int *arr, int n)
{
	int result = arr[0];
	for (int i = 1; i < n; i++)
	{
		result = gcd(arr[i], result);

		if(result == 1)
		{
		return 1;
		}
	}
	return result;
}