# Arquitetura

## Estruturas de Dados 

### Technician (Doubly Linked List)

Utilizamos uma Doubly Linked List com um apontador (head) e uma variável 'count' para contagem do número atual de técnicos e uma variável 'next_id' que representa o próximo ID a ser utilizado.  


### Username do Technician (Hashmap)

Trabalho extra para valorização da nota final, implementamos 1 hashmap (username) com as colisões resolvidas por linked list. Obtendo assim uma eficiência de O(1) nas autenticação dos técnicos. 


### Equipment (Doubly Linked List)

Utilizamos uma Doubly Linked List com dois apontadores (head e tail) e uma variável 'count' para contagem do número atual de equipamentos e uma variável 'next_id' que representa o próximo ID a ser utilizado.  

#### Funções (Algoritmos)

|                   Função                 | Eficiência |
| :--------------------------------------- | :--------: |
| equipment_list_insert()                  | O(1)       |
| equipment_list_remove()                  | O(n)       |
| equipment_find_by_id/ip/mac()            | O(1)       |
| equipment_sort_by_status/type/location() | O(n log n) |


### ID/MAC/IP do Equipment (Hashmap)

Trabalho extra para valorização da nota final, implementamos então 3 hashmaps independentes (ID, IP, MAC) com as colisões resolvidas por linked list. Obtendo assim uma eficiência de O(1) nas funções de `find_by_id()`, `find_by_ip()` e `find_by_mac()`, hash function utilizamos o algoritmo DJB2.

A struct `hashmap_t` retorna um apontador genérico (void *), permitindo a reutilização para o `id_index`, `mac_index`, `ip_index` e `username_index` sem duplicação do código.

#### Funções (Algoritmos)

|            Função              | Eficiência |
| :----------------------------- | :--------: |
| hashmap_insert()               | O(1)       |
| hashmap_get()                  | O(1)       |
| hashmap_remove()               | O(1)       |


Os hashmaps funcionam como index auxiliares para pesquisa mais eficiênte por ID, IP e MAC, embora seja raro com o algoritmo DJB2 haver colisões pode acontecer e no pior caso a eficiência é O(n).


### Incident (Queue && Singly Linked List)

Utilizamos uma Queue para representar os 'Incidents' em estado PENDING, com dois apontadores (head e tail) e uma variável 'count' para contagem do número atual de incidentes e uma variável 'next_id' que representa o próximo ID a ser utilizado. Os 'Incidents' com estado IN_PROGRESS e CONCLUDE são adicionados na linked list. 

#### Funções (Algoritmos)

|                 Função                | Eficiência |
| :------------------------------------ | :--------: |
| incident_queue_enqueue()              | O(1)       |
| incident_queue_dequeue()              | O(1)       |
| incident_list_insert()                | O(1)       |
| incident_queue_peek_head()            | O(1)       |
| incident_find_by_equipment_sensor()   | O(n)       |
| incident_filter_by_priority()         | O(n)       |


### Config (Stack)

Utilizamos uma Stack com um apontador (top) e uma variável 'count' para contagem do número de configurações.

#### Funções (Algoritmos)

|            Função              | Eficiência |
| :----------------------------- | :--------: |
| config_stack_push()            | O(1)       |
| config_stack_pop()             | O(1)       |
| config_stack_peek()            | O(1)       |
| config_filter_by_equipment()   | O(n)       |


### Sensor  (Singly Linked List)

Utilizamos uma Singly Linked List com um apontador (head) e uma variável 'count' para contagem do número de leituras dos sensores. 

#### Funções (Algoritmos)

|            Função              | Eficiência |
| :----------------------------- | :--------: |
| sensor_list_insert()           | O(1)       |
| sensor_filter_by_code()          | O(n)       |
| sensor_filter_anomalous()      | O(n)       |


## Algoritmos (Sorting, Binary e Linear Search)

### Sorting

Utilizamos o Merge Sort, mostrando maior eficiência em relação aos outros algoritmos de ordenação como Bubble Sort, Selection Sort e Insertion Sort. Utilizamos o algortimo de slow/fast pointer para encontrar o node do meio, divindo a linked list em duas linked lists.

|            Função              | Eficiência |
| :----------------------------- | :--------: |
| equipment_sort_by_status()     | O(n log n) |
| equipment_sort_by_type()       | O(n log n) |
| equipment_sort_by_location()   | O(n log n) |


### Linear Search

Utilizamos Linear Search para o resto das operações de pesquisa, evitando assim excesso uso de memória.

|                 Função                | Eficiência |
| :------------------------------------ | :--------: |
| incident_filter_by_source_id()        | O(n)       |
| incident_filter_by_priority()         | O(n)       |
| config_filter_by_equipment()          | O(n)       |
| sensor_filter_by_code()               | O(n)       |
| sensor_filter_by_status()             | O(n)       |


## Layout

|     Ficheiro       |                          Descrição                            |
| :----------------- | :------------------------------------------------------------ |
| src/               | ficheiros .c, implementação da função.                        |
| inc/               | ficheiros .h, implementação do prototipo da função e structs. |
| ui/                | ficheiros .c e .h relacionados com a interface gráfica.       |
| data/              | ficheiros .txt .dat, relacionados com os dados.               |
| assets/            | imagens para interface gráfica da aplicação                   |
| application.h      | encapsulamento de todos dados                                 |
| technician.c       | singly linked list
| equipment.c        | doubly linked list + merge sort                               |
| hashmap.c          | username, ID, IP e MAC                                        |
| incident.c         | queue                                                         |  
| config.c           | stack                                                         |
| sensor.c           | singly linked list                                            |
| persistence.c      | ficheiros binarios e texto                                    |
| connectivity.c     | ping + parse do resultado                                     |
| reports.c          | gerar relatórios                                              |


## Convenções (Boas práticas)

O código deve ser consistênte e evitar misturar estilos para seguir as boas práticas, por isso utilizamos convenções padrão utilizadas na Linguagem C.

### Structs 

Utilizamos o sufixo `_t` e snake_case, como no exemplo:

```c 

typedef struct equipment_node_t {
    equipment_t data;
    struct equipment_node_t *next;
    struct equipment_node_t *previous;
} equipment_node_t;

```

### Enums

Utilizamos o sufixo `_t` e em SCREAMING_SNAKE_CASE, como no exemplo:

```c 

typedef enum {
    STATUS_OPERATIONAL,
    STATUS_FAILED,
    STATUS_MAINTENANCE,
    STATUS_DISABLED
} equipment_status_t;

```

### Funções

Utilizamos o prefixo do módulo + estrutura de dados + operação e em snake_case, como no exemplo:

```c 

equipment_list_insert()
equipment_list_remove()
equipment_list_find_by_ip()

incident_queue_enqueue()
incident_queue_dequeue()

```

### Variáveis

Utilizamos snake_case, como no exemplo:

```c 

char ip_address[16];
char mac_address[18];

```

### Macros

Utilizamos SCREAMING_SNAKE_CASE, como no exemplo:

```c 

#define STRING_MAX     100
#define IP_MAX          16
#define MAC_MAX         18
#define PASSWORD_MAX   256

```


## Interface Gráfica

A implementação da interface gráfica (GUI) é feita com recurso ao GTK4 com personalização realizada em CSS (style.css), a interface foi inspirada em aplicações com o mesmo foco como o OpManager da ManageEngine e o N-central da N-able.

![example](example.png)

No exemplo do design da interface, podemos verificar a posição do logotipo no canto superior esquerdo, o atual utilizador em sessão no canto superior direito, logo de seguinda em baixo vemos o menu/topbar com as opções de acesso aos módulos. 

O menu/topbar é utilizado o widget `GtkBox` com o modo `GTK_ORIENTATION_HORIZONTAL`, sendo inserido os botões que representam os módulos.

É utilizado o widget `GtkStack` para controlar o acesso a esses módulos cada um inserido numa `GtkStackPage`. 

Para as listas de equipamentos e incidentes é utilizado uma forma manual de exibição, consistindo em dois loops `for`. O primeiro loop representa as linhas da lista (número total de equipamentos) e o segundo loop representa as colunas da lista (elementos de cada equipamento.), cada interação é inserido num `GtkGrid`.


### Cores

Cores utilizadas no design da aplicação:

* #19c37d (Verde, cor principal)
* #171717 (Preto, utilizado no topbar e como cor secundária)
* #f5f6f8 (Branco, utilizado como background)
* #ff4d4f (Vermelho, utilizado como estado 'Critical')
* #faad14 (Amarelo, utilizado como estado 'Warning')
* #52c41a (Verde, utilizado como estado 'Operational')


## Referências

https://docs.gtk.org/gtk4/
https://theartincode.stanis.me/008-djb2/
https://man7.org/linux/man-pages/man2/time.2.html
https://man7.org/linux/man-pages/man3/localtime.3p.html
https://man7.org/linux/man-pages/man3/strftime.3.html
https://medium.com/@arifimran5/fast-and-slow-pointer-pattern-in-linked-list-43647869ac99
https://www.baeldung.com/cs/merge-sort-linked-list
https://stackoverflow.com/questions/840501/how-do-function-pointers-in-c-work
https://www.cs.cmu.edu/~ab/15-123N09/lectures/Lecture%2008%20-%20Function%20Pointers.pdf
