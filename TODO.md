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
- [X] application_init() / application_destroy()

### Hashmap - Responsável:
- [X] Criar struct hashmap_t
- [X] Implementar DJB2 hash function 
- [X] hashmap_insert() / hashmap_remove()
- [X] hashmap_get()
- [X] hashmap_init() / hashmap_destroy()

### Technician - Responsável:
- [X] Criar structs (technician_t, technician_node_t, technician_list_t)
- [X] technician_list_insert()
- [X] technician_list_remove()
- [ ] technician_list_find_by_username()
- [X] Integrar username_index (hashmap)
- [X] technician_list_init() / technician_list_destroy()

### Equipment - Responsável:
- [X] Criar structs (equipment_t, equipment_node_t, equipment_list_t)
- [X] equipment_list_insert()
- [X] equipment_list_remove()
- [X] equipment_list_update_name/vendor/type/ip_address()...
- [ ] equipment_find_by_id/ip/mac()
- [ ] equipment_sort_by_status/type/location() - (Merge Sort)
- [X] Integrar id_index, ip_index, mac_index (hashmap)
- [X] equipment_list_init() / equipment_list_destroy()

### Incident - Responsável:
- [X] Criar structs (incident_t, incident_node_t, incident_queue_t, incident_list_t)
- [X] incident_queue_enqueue()
- [X] incident_queue_dequeue()
- [X] incident_list_insert()
- [ ] incident_queue_peek_front()
- [ ] incident_list_conclude()
- [ ] incident_filter_by_source()
- [ ] incident_filter_by_priority()
- [ ] incident_filter_by_status()
- [X] incident_queue_init() / incident_queue_destroy()
- [X] incident_list_init() / incident_list_destroy()

### Config - Responsável:
- [X] Criar structs  (config_t, config_node_t, config_stack_t)
- [X] config_stack_push()
- [X] config_stack_pop()
- [X] config_stack_peek()
- [ ] config_filter_by_equipment()
- [X] config_stack_init() / config_stack_destroy()

### Sensor - Responsável:
- [X] Criar structs (sensor_t, sensor_node_t, sensor_list_t)
- [X] sensor_list_insert()
- [ ] sensor_find_by_code()
- [ ] sensor_filter_anomalous()
- [X] sensor_list_init() / sensor_list_destroy()

### Reports - Responsável
- [ ] Relatório de estado da rede
- [ ] Relatório mensal de incidentes 


## Persistence + Connectivity + Logger

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

### Logger - Responsável:
- [ ] Implementar log system (audit.log e system.log)


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
