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

### Application - Responsável: Daniel
- [X] Criar struct application_t 
- [X] application_init() / application_destroy()

### Hashmap - Responsável: Daniel
- [X] Criar struct hashmap_t
- [X] Implementar DJB2 hash function 
- [X] hashmap_insert() / hashmap_remove()
- [X] hashmap_get()
- [X] hashmap_init() / hashmap_destroy()

### Technician - Responsável: Daniel
- [X] Criar structs (technician_t, technician_node_t, technician_list_t)
- [X] technician_list_insert()
- [X] technician_list_remove()
- [X] technician_filter_by_role()
- [X] Integrar username_index (hashmap)
- [X] technician_list_init() / technician_list_destroy()

### Equipment - Responsável: Daniel
- [X] Criar structs (equipment_t, equipment_node_t, equipment_list_t)
- [X] equipment_list_insert()
- [X] equipment_list_remove()
- [X] equipment_list_update_name/vendor/type/ip_address()...
- [X] equipment_filter_by_status/type()
- [X] equipment_sort_by_status/type/location() - (Merge Sort)
- [X] Integrar id_index, ip_index, mac_index (hashmap)
- [X] equipment_list_init() / equipment_list_destroy()

### Incident - Responsável: Daniel
- [X] Criar structs (incident_t, incident_node_t, incident_queue_t, incident_list_t)
- [X] incident_queue_enqueue()
- [X] incident_queue_dequeue()
- [X] incident_list_insert()
- [X] incident_queue_peek()
- [X] incident_list_conclude()
- [X] incident_filter_by_priority/status/source_id()
- [X] incident_queue_init() / incident_queue_destroy()
- [X] incident_list_init() / incident_list_destroy()

### Config - Responsável: Daniel
- [X] Criar structs  (config_t, config_node_t, config_stack_t)
- [X] config_stack_push()
- [X] config_stack_pop()
- [X] config_stack_peek()
- [X] config_filter_by_equipment()
- [X] config_stack_init() / config_stack_destroy()

### Sensor - Responsável: Daniel
- [X] Criar structs (sensor_t, sensor_node_t, sensor_list_t)
- [X] sensor_list_insert()
- [X] sensor_filter_by_code()
- [X] sensor_filter_status()
- [X] sensor_list_init() / sensor_list_destroy()


## Persistence + Connectivity + Logger

### Persistence - Responsável: Daniel
- [X] Implementar save/load_technician()
- [X] Implementar save/load_equipment()
- [X] Implementar save/load_incident()
- [X] Implementar save/load_config()
- [X] Implementar save/load_sensor()

### Connectivity - Responsável: Daniel
- [X] Implementar ping()
- [ ] Implementar traceroute()
- [X] Implementar parse do ping()

### Logger - Responsável: Daniel
- [ ] Implementar log system (audit.log e system.log)


## Interface Gráfica (GTK4)
- [X] Login
- [ ] Dashboard
- [ ] Módulo Technician
- [X] Módulo Equipment
- [X] Módulo Incident
- [X] Módulo Config
- [X] Módulo Sensor
- [X] Módulo Connectivity


## Testes 
- [X] Testar manualmente cada módulo e a interface
- [X] Testar memory leaks com Valgrind/GDB


## Relatório (LaTeX)
- [ ] Criar estrutura (índice, cápitulos/secções) - Responsável:
- [ ] Introdução e descrição do problema/projeto - Responsável:
- [ ] Documentar data structures e algoritmos utilizados - Responsável:
- [ ] Documentar decisões de arquitetura - Responsável:
- [ ] Conclusão - Responsável:
