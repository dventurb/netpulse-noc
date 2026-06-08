#ifndef PAGINATION_H
#define PAGINATION_H

typedef struct {
  int total;
  int page;
  int page_size;
} pagination_t;

void pagination_get_range(pagination_t pagination, int *start, int *end);
void pagination_set_page_number(pagination_t *pagination, int number);
void pagination_previous(pagination_t *pagination);
void pagination_next(pagination_t *pagination);

int pagination_total_pages(pagination_t pagination, int count);
int pagination_start(pagination_t pagination);
int pagination_end(pagination_t pagination);

#endif
