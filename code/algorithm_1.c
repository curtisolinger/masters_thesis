#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

// Define the max element length
const int N = 101;

struct node {
    int element;
    int coef[6];
    int length;
    struct node *next;
};

void hash_monoid(struct node **arr, struct node *list, int M_MAX, int E);
void hash_element_arr(struct node **arr0, struct node **arr1, int M_MAX);
void free_list(struct node *list);
int size(struct node *ptr);
void bubble_sort(int *arr, int n);
int gcd(int a, int b);
int findGCD(int *arr, int n);
void print_gen_set(int *arr, int E);
void create_gen_set(int *arr, int max, int min, int E);
void free_dec(struct node **n);
struct node *create_monoid(int *arr, int M_MAX, int M, int E);
void free_hash(struct node **arr, int M_MAX);
void print_to_file(struct node **arr0, int *arr1, int min, int max, int E);
bool gen_duplicate(int x, int *arr, int end);

int main(void)
{
	clock_t start, end;
	double cpu_time_used;
	start = clock();

	// Declare the embedding dimension variable E
	int i, j, k, l, E, index, min, max, num_range;

	num_range = 3;

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

	int arr1[] = {6};
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
			const int M_MAX = (N*gen_set[E - 1]) - gen_set[0] + 1;
			printf("M_MAX: %i\n", M_MAX);

			// Define the number of times to loop through each generator.
			// LOOP_MAX must be large enough so that numbers less than M_MAX have their max decomposition calculated. 
			// E.G. If M = <2,3>, then M_MAX = 302. Notice 298 = 149*2 + 0*3. Thus 298 has length of 149. But that wont be 
			// calculated if loop only goes through N. So LOOP_MAX = 151. 
		    const int LOOP_MAX = N*gen_set[E-1] / gen_set[0];
			printf("LOOP_MAX: %i\n", LOOP_MAX);

			// Build the monoid from 0 to M_MAX
			struct node *monoid = create_monoid(gen_set, M_MAX, LOOP_MAX, E);
			
			// Hash monoid by element length, only keeping max decomposition length.
			// The i'th element of the array contains a pointer to the node with the maximum decomposition of element i. 
			struct node *element_arr[M_MAX];
			hash_monoid(element_arr, monoid, M_MAX, E);

		    // Hash element_arr by length. The i'th element of the array contains a pointer to a linked list of nodes
		    // who all have a length of i. 
		    struct node *length_arr[N];
		   	hash_element_arr(length_arr, element_arr, M_MAX);

		    // Print k and d_k(M) to csv file 
		    print_to_file(length_arr, gen_set, min, max, E);

		    free_dec(length_arr);
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

// Function to build the numerical semigroup linked list
struct node *create_monoid(int *arr, int M_MAX, int M, int E)
{
	int i, j, k, l, m, o;
	struct node *n;
	struct node *list;
	list = NULL;
    if (E == 2) {
        for (i = 0; i < M; i++) {
            for (j = 0; j < M; j++) {
				if (i*arr[0] + j*arr[1] < M_MAX) {
                	n = malloc(sizeof(*n));
                	if (!n) {
                    	free_list(list);
                    	printf("ERROR\n");
                	}
                	n -> element = i*arr[0] + j*arr[1];
                	n -> coef[0] = i;
                	n -> coef[1] = j;
                	n -> length = i + j;
                	n -> next = list;
                	list = n;
                }
            }
        }
    }
    else if (E == 3) {
        for (i = 0; i < M; i++) {
            for (j = 0; j < M; j++) {
                for (k = 0; k < M; k++) {
                    if (i*arr[0] + j*arr[1] + k*arr[2] < M_MAX) {
                    	n = malloc(sizeof(*n));
	                    if (!n) {
	                        free_list(list);
	                        printf("ERROR\n");
	                    }
	                    n -> element = i*arr[0] + j*arr[1] + k*arr[2];
	                    n -> coef[0] = i;
	                	n -> coef[1] = j;
	                	n -> coef[2] = k;
	                    n -> length = i + j + k;
	                    n -> next = list;
                		list = n;
                    }
                }
            }
        }
    }
    else if (E == 4) {
        for (i = 0; i < M; i++) {
            for (j = 0; j < M; j++) {
                for (k = 0; k < M; k++) {
                    for (l = 0; l < M; l++) {
                        if (i*arr[0] + j*arr[1] + k*arr[2] + l*arr[3] < M_MAX) {
                        	n = malloc(sizeof(*n));
	                        if (!n) {
	                            free_list(list);
	                            printf("ERROR\n");
	                        }
	                        n -> element = i*arr[0] + j*arr[1] + k*arr[2] + l*arr[3];
	                        n -> coef[0] = i;
	                		n -> coef[1] = j;
	                		n -> coef[2] = k;
	                		n -> coef[3] = l;
	                        n -> length = i + j + k + l;
	                        n -> next = list;
                			list = n;
	                    }
                    }
                }
            }
        }
    }
    else if (E == 5) {
        for (i = 0; i < M; i++) {
            for (j = 0; j < M; j++) {
                for (k = 0; k < M; k++) {
                    for (l = 0; l < M; l++) {
                        for (m = 0; m < M; m++) {
                            if (i*arr[0] + j*arr[1] + k*arr[2] + l*arr[3] + m*arr[4] < M_MAX) {
                            	n = malloc(sizeof(*n));
	                            if (!n) {
	                                free_list(list);
	                                printf("ERROR\n");
	                            }
	                            n -> element = i*arr[0] + j*arr[1] + k*arr[2] + l*arr[3] + m*arr[4];
	                            n -> coef[0] = i;
	                			n -> coef[1] = j;
	                			n -> coef[2] = k;
	                			n -> coef[3] = l;
	                			n -> coef[4] = m;
	                            n -> length = i + j + k + l + m;
	                            n -> next = list;
                				list = n;
                            }
                        }
                    }
                }
            }
        }
    }
        else if (E == 6) {
        for (i = 0; i < M; i++) {
            for (j = 0; j < M; j++) {
                for (k = 0; k < M; k++) {
                    for (l = 0; l < M; l++) {
                        for (m = 0; m < M; m++) {
                        	for (o = 0; o < M; o++) {                         
	                            if (i*arr[0] + j*arr[1] + k*arr[2] + l*arr[3] + m*arr[4] + o*arr[5] < M_MAX) {
	                            	n = malloc(sizeof(*n));
		                            if (!n) {
		                                free_list(list);
		                                printf("ERROR\n");
		                            }
		                            n -> element = i*arr[0] + j*arr[1] + k*arr[2] + l*arr[3] + m*arr[4] + o*arr[5];
		                            n -> coef[0] = i;
		                			n -> coef[1] = j;
		                			n -> coef[2] = k;
		                			n -> coef[3] = l;
		                			n -> coef[4] = m;
		                			n -> coef[5] = o;
		                            n -> length = i + j + k + l + m + o;
		                            n -> next = list;
	                				list = n;
	                            }
                        	}
                        }
                    }
                }
        	}
    	}
    }
    else
    {
        printf("Incorrect number of generators\n");
        printf("ERROR\n");
    }
	return list;
}

// hash_monoid(element_arr, monoid, M_MAX, int E);
void hash_monoid(struct node **arr, struct node *list, int M_MAX, int E)
{
	int i, x;
	struct node *tmp = NULL;
	for (i = 0; i < M_MAX; i++)
		arr[i] = NULL;
    while (list != NULL)
    {
        tmp = list;
        list = tmp -> next;
        x = tmp -> element;
        if (x < M_MAX && arr[x] == NULL) {
        	arr[x] = tmp;
        	tmp -> next = NULL;
        }
        else if (x < M_MAX && tmp -> length > arr[x] -> length) {
        	free(arr[x]);
        	arr[x] = tmp;
        	tmp -> next = NULL;
        }
        else {
        	free(tmp);
        }
    }
}



// hash_element_arr(length_arr, element_arr, M_MAX);
void hash_element_arr(struct node **arr0, struct node **arr1, int M_MAX)
{
    int i, x;
    struct node *tmp = NULL;
    for (i = 0; i < N; i++)
		arr0[i] = NULL;
    for (i = 0; i < M_MAX; i++) {

		if (arr1[i] != NULL) {
			tmp = arr1[i];
			x = tmp -> length;
			if (x < N) {
				tmp -> next = arr0[x];
				arr0[x] = tmp;
			}
			else {
				free(tmp);
			}
		}
	}
}

int size(struct node *ptr)
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

void free_list(struct node *list)
{
    struct node *tmp;
    while (list != NULL)
    {
        tmp = list -> next;
        free(list);
        list = tmp;
    }
}

void free_dec(struct node **n)
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

void free_hash(struct node **arr, int M_MAX) {
    struct node *tmp0, *tmp1;
    for (int i = 0; i < M_MAX; i++) {
        tmp0 = arr[i];
        while (tmp0 != NULL) {
            tmp1 = tmp0 -> next;
            free(tmp0);
            tmp0 = tmp1;
        }
    }
}

// print_to_file(length_arr, gen_set, min, max, E);
void print_to_file(struct node **arr0, int *arr1, int min, int max, int E)
{
	int i;
	
	char buffer0[50];
	sprintf(buffer0, "output/%i_%i_%i", min, max, E);
	char buffer1[5];
	for (i = 0; i < E; i++) {
		sprintf(buffer1, "_%i", arr1[i]);
		strncat(buffer0, buffer1, 5);
	}
	sprintf(buffer1, ".csv");
	strncat(buffer0, buffer1, 5);

	FILE *file = fopen(buffer0, "w");
    if (file == NULL){
        printf("Could not open file, line %d in function %s.\n", __LINE__, __func__);
    }
    fprintf(file, "k, d_k(M)\n");
    for (i = 0; i < N; i++)
    	fprintf(file, "%i, %i\n", i, size(arr0[i]));
    fclose(file);
}