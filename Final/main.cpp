#include <stdio.h>

#include "parameter.h"
#include "test_label.h"
#include "test_set.h"
#include "training_label.h"
#include "training_set.h"
//#define TRAIN

#ifdef TRAIN
w_t *input = train_set;
w_t *label = train_label;
#else
w_t *input = test_set;
w_t *label = test_label;
#define TESTNUM 2000
#endif

extern w_t scoresheet[SIZE * SIZE];
extern w_t border[SIZE * SIZE];
extern w_t average[SIZE * SIZE];

w_t image[SIZE * SIZE];

#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4


void check(int iter, int score)
{
	static int wrongs = 0;
	//printf("#%d, Pred : %d, Label : %d\n", iter, score, label[iter]);
	if (score != label[iter])
		wrongs++;
	if (iter == TESTNUM - 1)
		printf("Accuracy : %.2f%%\n", 100 - 100.0 * wrongs / TESTNUM);
}

int main()
{
	/*
	1. subtract average from image.
	2. pick top 3 pixels,
		2-1. if the pixel is on border,
		2-2. investigate LEFT/RIGHT/UP/DOWN pixels, and pick max.
		2-3. make all 9 pixels zero.
		2-4. assign score of the point(if border, the neighbor).
	*/
	for (int iter = 0; iter < TESTNUM; iter++)
	{
		//1. subtract average from image
		for (int i = 0; i < SIZE*SIZE; i++)
		{
			w_t value = 255 - input[i + iter*SIZE*SIZE] - average[i];
			image[i] = value;
			//image[i] = value > 20 ? value : 0;
		}

		int score = 0;
		for (int point = 0; point < 3; point++)
		{
			//find maximum point!
			w_t max_value = 0;
			int max_index = 0;
			for (int i = 0; i < SIZE*SIZE; i++)
			{
				if (image[i] > max_value)
				{
					max_value = image[i];
					max_index = i;
				}
			}
			bool onborder = border[max_index];

			int value_index = max_index;
			//2-1. if the pixel is on border
			if (onborder)
			{
				w_t left = image[max_index - 1];
				w_t right = image[max_index + 1];
				w_t up = image[max_index - SIZE];
				w_t down = image[max_index + SIZE];
				//max_value = down;
				value_index = max_index + SIZE;
				//if pixel on right border
				if (max_index%SIZE == SIZE - 1)
				{
					if (left >= up && left >= down)
						value_index = max_index - 1;
					else if (up >= left && up >= down)
						value_index = max_index - SIZE;
				}
				//if pixel on top
				else if (max_index < SIZE)
				{
					if (left >= right && left >= down)
						value_index = max_index - 1;
					else if (right >= left && right >= down)
						value_index = max_index + 1;
				}
				//if pixel not on top
				else
				{
					if (left >= right && left >= up && left >= down)
						value_index = max_index - 1;
					else if (right >= left && right >= up && right >= down)
						value_index = max_index + 1;
					else if (up >= left && up >= right && up >= down)
						value_index = max_index - SIZE;
				}
			}

			//2-3. make all 9 pixels zero.
			image[max_index - 1] = 0;
			image[max_index] = 0;
			image[max_index + SIZE - 1] = 0;
			image[max_index + SIZE] = 0;
			
			if (max_index >= SIZE)
			{
				image[max_index - SIZE - 1] = 0;
				image[max_index - SIZE] = 0;				

				if (max_index%SIZE == SIZE - 1)
				{
					image[max_index - SIZE + 1] = 0;
					image[max_index + 1] = 0;
					image[max_index + SIZE + 1] = 0;
				}
			}
			else if (max_index%SIZE == SIZE - 1)
			{
				image[max_index + 1] = 0;
				image[max_index + SIZE + 1] = 0;
			}

			//2-4. assign score.
			score += scoresheet[value_index];
		}
		check(iter, score);
	}
	return 0;
}

