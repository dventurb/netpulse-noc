# Arquitetura

## Estruturas de Dados 

### Technician (Doubly Linked List)

Utiliza-se uma doubly linked list com um apontador para o início (`head`) e uma variável `count` para monitorizar o número atual de técnicos registados. O controlo de registo é gerido através de uma variável `next_id`, armazenado o próximo identicador único.


### Username do Technician (Hashmap)

Como elemento de valorização extra, implementou-se uma hashmap (`username_index`) com as colisões resolvidas por uma singly linked list (chaining). Esta estrutura auxiliar garante uma eficiência de **O(1)** nas autenticação dos técnicos e sua validação. 


### Equipment (Doubly Linked List)

Utiliza-se uma doubly linked list com dois apontadores (`head` e `tail`), e uma variável `count` para contagem do número atual de equipamentos e uma variável `next_id` para gerar identicador único de forma incremental.

#### Eficiência das Funções

|                   Função                 | Eficiência |                    Descrição                    |
| :--------------------------------------- | :--------: | :---------------------------------------------  |
| equipment_list_insert()                  | O(1)       | Inserção imediata utilizando o apontador `tail`.|
| equipment_list_remove()                  | O(n)       | Remoção com pesquisa linear prévia do `node`.   |
| equipment_find_by_id/ip/mac()            | O(1)       | Resultado instanâneo através dos hashmaps.      |
| equipment_sort_by_status/type/location() | O(n log n) | Ordenação recorrendo ao algoritmo Merge Sort.   |


### ID/MAC/IP do Equipment (Hashmap)

Para valorização extra, foram implementadas três hashmaps independentes (`id_index`, `ip_index`, `mac_index`). As colisões são resolvidas através de linked list (chaining), a geração da `key` é  processada pelo algoritmo de hash **DJB2**.

A struct `hashmap_bucket_t` foi implementada de forma polimórfica utilizando um apontador genérico (void *), permitindo a reutilização para os dados dos técnicos (`username_index`), evitando duplicação do código.

#### Eficiência das Funções

|            Função              | Eficiência |
| :----------------------------- | :--------: |
| hashmap_insert()               | O(1)       |
| hashmap_get()                  | O(1)       |
| hashmap_remove()               | O(1)       |


Nota: O pior caso (O(n)) apenas occore em cenários de saturação de colisões do mesmo bucket, embora seja mitigado com a distribuição do algoritmo **DJB2**, vai sempre depender da relação do número de equipamentos e o tamanho do `hashmap_bucket_t`.


### Incident (Queue && Singly Linked List)

Os incidentes em estado `PENDING` são organizados através da queue, suportada por dois apontadores (`head` e `tail`), uma variável `count` para contagem do número atual de incidentes e uma variável `next_number` para gestão incremental. Assim que um incidente transita para os estados `IN_PROGRESS` e `CONCLUDE`, são movidos para uma singly linked list.

#### Eficiência das Funções

|                 Função                | Eficiência |                    Descrição                    |
| :------------------------------------ | :--------: | :---------------------------------------------  |
| incident_queue_enqueue()              | O(1)       | Inserção no `rear` da queue.                    |
| incident_queue_dequeue()              | O(1)       | Remoção e processamento no `front` da queue.    |
| incident_list_insert()                | O(1)       | Inserção na `head` da linked list.              |
| incident_find_by_equipment_sensor()   | O(n)       | Pesquisar linear nas duas estruturas de dados.  |
| incident_filter_by_priority()         | O(n)       | Filtragem linear nas duas estruturas de dados.  |


### Configuration (Stack)

As configurações dos equipamentos são organizados através de uma stack, gerido por um apontador `top` e contador do número `count`.

#### Eficiência das Funções

|            Função              | Eficiência |                    Descrição                    |
| :----------------------------- | :--------: | :---------------------------------------------  |
| config_stack_push()            | O(1)       | Inserção no `top` da stack.                     |
| config_stack_pop()             | O(1)       | Remoção no `top` da stack.                      |


### Sensor  (Dynamic Array)

Diferente das abordagens anteriores, o armazenamento é realizado em tempo de utilização através de um array dinâmico (`sensor_t`). Esta reestrutura permite garantir eficiência devido ao uso da cache da CPU em memória contígua e permite indexação direta por índices.

#### Eficiência das Funções

|            Função              | Eficiência |                    Descrição                    |
| :----------------------------- | :--------: | :---------------------------------------------  |
| sensor_persistence_append()    | O(n)       | Inserção e realocação dos novos dados.          |
| sensor_filter_by_code()        | O(n)       | Filtragem linear.                               |


## Algoritmos (Sorting, Binary e Linear Search)

### Sorting

Utiliza-se o algoritmo **Merge Sort**, mostrando maior eficiência em relação aos outros algoritmos de ordenação como **Bubble Sort**, **Selection Sort** e **Insertion Sort**. Recorrendo ao algoritmo de Slow/Fast Pointers para encontrar o node do meioda linked list, divindo assim em duas linked lists.

|            Função              | Eficiência |
| :----------------------------- | :--------: |
| equipment_sort_by_status()     | O(n log n) |
| equipment_sort_by_type()       | O(n log n) |
| equipment_sort_by_location()   | O(n log n) |

### Linear Search

Aplica-se diretamente sobre o ficheiro de persistência (`sensors.bin`) que armazenam o timestamp (`time_t`), em vez de carregar múltiplos registos de sensores para memória, utiliza-se o algoritmo de pesquisa binária em disco, recorrendo a deslocações utilizando `fseek` e `ftell`.

|            Função              | Eficiência |
| :----------------------------- | :--------: |
| sensor_search_by_date()        |  O(log n)  |


### Linear Search

A pesquisa linear é utilizada em operações de filtragem e em estruturas onde não justificava a manutenção de um índice.

|                 Função                | Eficiência |
| :------------------------------------ | :--------: |
| incident_filter_by_source_id()        | O(n)       |
| incident_filter_by_priority()         | O(n)       |
| sensor_filter_by_code()               | O(n)       |
| sensor_filter_by_status()             | O(n)       |


## Coding Style (Boas Práticas)

Para assegurar legibilidade e manutenção, adotou-se convenções utilizadas na Linguagem C.

### Comentários 

Evitou-se ao máximo o uso de comentários no projeto, o código deve ser totalmente claro e fácil de entender, priorizando a legibilidade através de escolha de nomes para variáveis, funções e estruturas. Não devem ser escritos comentários de linha única que servem apenas para descrever o óbvio ou o que a linha seguinte faz, o código deve falar por si mesmo.

#### Uso Incorrento 

```c 

  // Obtém a data da leitura do sensor
  char *data = sensors->read_at;

  // Verifica se a data é correta
  if (validate_date(date)) {
      // ...
  }

```

#### Uso Correto

```c 

  char *data = sensors->read_at;

  if (validate_date(date)) {
      // ...
  }

```

### Structs 

Utiliza-se `snake_case` e o sufixo `_t` para definição das structures, como no exemplo:

```c 

typedef struct equipment_node_t {
    equipment_t data;
    struct equipment_node_t *next;
    struct equipment_node_t *previous;
} equipment_node_t;

```

### Enums

Utiliza-se `SCREAMING_SNAKE_CASE` e o sufixo `_t`, como no exemplo:

```c 

typedef enum {
    STATUS_OPERATIONAL,
    STATUS_FAILED,
    STATUS_MAINTENANCE,
    STATUS_DISABLED
} equipment_status_t;

```

### Funções

Encapsulamento pelo padrão: `prefixo_módulo` + `estrutura_de_dados` + `operação`, em `snake_case`, como no exemplo:

```c 

equipment_list_insert()
equipment_list_remove()
equipment_list_find_by_ip()

incident_queue_enqueue()
incident_queue_dequeue()

```

### Variáveis

Utiliza-se `snake_case` e bem descritivas do que representam, como no exemplo:

```c 

char ip_address[16];
char mac_address[18];

technician_t *current_user;

```

### Macros

Utiliza-se `SCREAMING_SNAKE_CASE`, como no exemplo:

```c 

#define STRING_MAX     100
#define IP_MAX          16
#define MAC_MAX         18
#define PASSWORD_MAX   256

```


## Interface Gráfica (GUI)

A implementação da visualização gráfica é feita com recurso a biblioteca **GTK4**, com personalização realizada em CSS (`style.css`). A interface foi inspirada em aplicações com o mesmo foco, tais como o ManageEngine OpManager e o N-able N-central.

![example](https://i.imgur.com/En0IFrp.png)


* **Login Screen:** Após a validação com sucesso, a `GtkWindow` é destruída e a memória alocada dinamicamente `login_window_t` é libertada com recurso ao `g_object_set_data_full()`.

* **Menu Principal:** Utiliza-se um container horizontal `GtkBox` (`GTK_ORIENTATION_HORIZONTAL`) para servir como barra de navegação durante o uso da aplicação.

* **Navegação em Páginas:** A mudança entre as páginas de cada módulo é gerida pelo widget `GtkStack`, onde cada submódulo está isolado dentro de uma `GtkStackPage`.

* **Tabela de Dados:** A representação visual dos dados (como as tabelas de equipamentos e incidentes) é efetuada através de dois loops `for`. Os elementos da tabela são inseridos de forma linear dentro de um widget `GtkGrid`.


### Cores

* `#19C37D`: Verde NetPulse (Cor Primária)
* `#171717`: Grafite Escuro (Topbar e Cor Secundária)
* `#F5F6F8`: Cinza Claro (Background)
* `#FF4D4F`: Vermelho (Representação de Estados `CRITICAL`)
* `#FAAD14`: Amarelo (Representação de Estados `WARNING`)
* `#52C41A`: Verde (Representação de Estados `OPERATIONAL`)


## Referências

* **GTK 4:** https://docs.gtk.org/gtk4/
* **DJB2 Hash Algorithm:** https://theartincode.stanis.me/008-djb2/
* **POSIX Threads (`pthreads`):** https://man7.org/linux/man-pages/man7/pthreads.7.html
* **Multithreading:** https://www.geeksforgeeks.org/c/multithreading-in-c/
* **Multithreading:** https://dev.to/quantumsheep/basics-of-multithreading-in-c-4pam
* **File I/O Operations:** https://man7.org/linux/man-pages/man3/fopen.3.html
* **Fast and Slow Pointer:** https://medium.com/@arifimran5/fast-and-slow-pointer-pattern-in-linked-list-43647869ac99
* **Merge Sort:** https://www.baeldung.com/cs/merge-sort-linked-list
* **Function Pointers:** https://stackoverflow.com/questions/840501/how-do-function-pointers-in-c-work
* **Function Pointers:** https://www.cs.cmu.edu/~ab/15-123N09/lectures/Lecture%2008%20-%20Function%20Pointers.pdf
* **time():** https://man7.org/linux/man-pages/man2/time.2.html
* **localtime():** https://man7.org/linux/man-pages/man3/localtime.3p.html
* **strftime():** https://man7.org/linux/man-pages/man3/strftime.3.html
* **Makefile OS-Decting:** https://stackoverflow.com/questions/714100/os-detecting-makefile
* **Makefile Cross-Platform:** https://linuxvox.com/blog/makefile-that-distinguishes-between-windows-and-unix-like-systems/
* **IP Validation:** https://stackoverflow.com/questions/791982/determine-if-a-string-is-a-valid-ipv4-address-in-c
* **sscanf():** https://www.tutorialspoint.com/c_standard_library/c_function_sscanf.htm
* **Ping on Windows:** https://stackoverflow.com/questions/60992843/how-to-perform-a-ping-test-programmatically-on-windows
