#include "pagination.h"

void pagination_get_range(pagination_t pagination, int *start, int *end)
{
  int total = pagination_total_pages(pagination, pagination.total);
  if (total <= 0) total = 1;

  *start = pagination.page - 1;
  *end = pagination.page + 1;

  if (*start < 0) *start = 0;
  if (*end > total - 1) *end = total - 1;
}

void pagination_set_page_number(pagination_t *pagination, int number)
{
  pagination->page = number;
}

void pagination_previous(pagination_t *pagination)
{
  pagination->page--;

  if (pagination->page < 0) 
    pagination->page = 0; 
}

void pagination_next(pagination_t *pagination)
{
  int total = pagination_total_pages(*pagination, pagination->total);

  pagination->page++;

  if (pagination->page > total - 1) 
    pagination->page = total - 1; 

  if (pagination->page < 0)
    pagination->page = 0;
}

int pagination_total_pages(pagination_t pagination, int count)
{
  return (count + pagination.page_size - 1) / pagination.page_size;
}

int pagination_start(pagination_t pagination)
{
  return pagination.page * pagination.page_size;
}

int pagination_end(pagination_t pagination)
{
  return pagination.page * pagination.page_size + pagination.page_size;
}
