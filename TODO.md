# TODO.md 

## Arquitetura + Design
- [X] Definir quais data structures utilizar
- [X] Detalhar a arquitetura do sistema (ARCHITECTURE.md)
- [X] Criar classDiagram com a relação entre entidades (Mermaid)
- [X] Criar mockups da interface gráfica (Figma)
- [ ] Criar sequence diagram (opcional)


## Structs + Headers (.h)
- [X] Definir todos os .h em conjunto. (.h antes dos .c)


## Módulos (Structures)

### Application - Responsável: 
- [X] Criar struct application_t 
- [ ] application_create() / application_free()

### Hashmap - Responsável:
- [X] Criar struct hashmap_t
- [X] Implementar DJB2 hash function 
- [X] hashmap_insert() / hashmap_remove()
- [X] hashmap_get()
- [ ] hashmap_create() / hashmap_free()

### Technician - Responsável:
- [X] Criar structs (technician_t, technician_node_t, technician_list_t)
- [X] technician_list_insert()
- [X] technician_list_remove()
- [ ] technician_list_find_by_id()
- [ ] Integrar username_index (hashmap)
- [ ] technician_list_create() / technician_list_free()

### Equipment - Responsável:
- [X] Criar structs (equipment_t, equipment_node_t, equipment_list_t)
- [X] equipment_list_insert()
- [X] equipment_list_remove()
- [ ] equipment_find_by_id/ip/mac()
- [ ] equipment_sort_by_status/type/location() (Merge Sort)
- [ ] Integrar id_index, ip_index, mac_index (hashmap)
- [ ] equipment_list_create() / equipment_list_free()

### Incident - Responsável:
- [X] Criar structs (incident_t, incident_node_t, incident_queue_t, incident_list_t)
- [X] incident_queue_enqueue()
- [X] incident_queue_dequeue()
- [X] incident_list_insert()
- [ ] incident_queue_peek_head()
- [ ] incident_find_by_equipment_sensor()
- [ ] incident_filter_by_priority()
- [ ] incident_queue_create() / incident_queue_free()
- [ ] incident_list_create() / incident_list_free()

### Config - Responsável:
- [X] Criar structs  (config_t, config_node_t, config_stack_t)
- [X] config_stack_push()
- [X] config_stack_pop()
- [X] config_stack_peek()
- [ ] config_stack_peek_n()
- [ ] config_filter_by_equipment()
- [ ] config_stack_create() / config_stack_free()

### Sensor - Responsável:
- [X] Criar structs (sensor_t, sensor_node_t, sensor_list_t)
- [X] sensor_list_insert()
- [ ] sensor_find_by_code()
- [ ] sensor_filter_anomalous()
- [ ] sensor_list_create() / sensor_list_free()


## Persistence + Connectivity

### Persistence - Responsável:
- [ ] Implementar save/load_technician()
- [ ] Implementar save/load_equipment()
- [ ] Implementar save/load_incident()
- [ ] Implementar save/load_config()
- [ ] Implementar save/load_sensor()

### Connectivity - Responsável:
- [ ] Implementar ping()
- [ ] Implementar traceroute()
- [ ] Implementar parse do ping()


## Interface Gráfica (GTK4)
- [ ] Login
- [ ] Dashboard
- [ ] Módulo Technician
- [ ] Módulo Equipment
- [ ] Módulo Incident
- [ ] Módulo Config
- [ ] Módulo Sensor
- [ ] Módulo Connectivity
- [ ] Importar/Exportar relatórios


## Testes 
- [ ] Testar manualmente cada módulo e a interface
- [ ] Testar memory leaks com Valgrind/GDB


## Relatório (LaTeX)
- [ ] Criar estrutura (índice, cápitulos/secções) - Responsável:
- [ ] Introdução e descrição do problema/projeto - Responsável:
- [ ] Documentar data structures e algoritmos utilizados - Responsável:
- [ ] Documentar decisões de arquitetura - Responsável:
- [ ] Conclusão - Responsável:
