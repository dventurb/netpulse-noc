#include "pagination.h"

static const int itens_per_page = 6;

void pagination_init(pagination_t *pagination, int total)
{
  pagination->current_page = 0;
  pagination->page_size = itens_per_page;
  pagination->total_items = total;
}

void pagination_get_range(pagination_t pagination, int *start, int *end)
{
  int total_pages = pagination_total_pages(pagination, pagination.total_items);
  if (total_pages <= 0) total_pages = 1;

  *start = pagination.current_page - 1;
  *end = pagination.current_page + 1;

  if (*start < 0) *start = 0;
  if (*end > total_pages - 1) *end = total_pages - 1;
}

void pagination_set_page_number(pagination_t *pagination, int number)
{
  pagination->current_page = number;
}

void pagination_previous(pagination_t *pagination)
{
  pagination->current_page--;

  if (pagination->current_page < 0) 
    pagination->current_page = 0; 
}

void pagination_next(pagination_t *pagination)
{
  int total_pages = pagination_total_pages(*pagination, pagination->total_items);

  pagination->current_page++;

  if (pagination->current_page >= total_pages) 
    pagination->current_page = total_pages - 1; 

  if (pagination->current_page < 0)
    pagination->current_page = 0;
}

int pagination_total_pages(pagination_t pagination, int count)
{
  return (count + pagination.page_size - 1) / pagination.page_size;
}

int pagination_start(pagination_t pagination)
{
  return pagination.current_page * pagination.page_size;
}

int pagination_end(pagination_t pagination)
{
  return pagination.current_page * pagination.page_size + pagination.page_size;
}

void pagination_fix_current_page(pagination_t *pagination, int total_pages)
{
  if (pagination->current_page >= total_pages - 1)
    pagination->current_page = total_pages - 1;

  if (pagination->current_page < 0)
    pagination->current_page = 0;
}
