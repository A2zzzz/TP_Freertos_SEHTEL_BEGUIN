# TP_Freertos_SEHTEL_BEGUIN

## 🟦 (Re)prise en main

### Où se situe le fichier main.c ?
Le fichier `main.c` se trouve dans le chemin suivant :  
`\TP_Freertos_SEHTEL_BEGUIN\Core\Src\main.c`

### À quoi servent les commentaires indiquant BEGIN et END ?
Les commentaires `BEGIN` et `END` délimitent les sections où l'on peut écrire du code sans que l'outil de génération de code (comme CubeMX) n'écrasent ces modifications lors d'une régénération.

### Quels sont les paramètres à passer à HAL_Delay et HAL_GPIO_TogglePin ?
- `HAL_Delay`: Le paramètre est un entier non signé `uint32_t` représentant le délai en millisecondes.
- `HAL_GPIO_TogglePin`: Les paramètres sont :
  - Le port GPIO.
  - Le numéro de la broche.

### Dans quel fichier les ports d’entrée/sorties sont-ils définis ?
Les ports d'entrée/sortie sont définis dans le fichier `gpio.c`.


##  FreeRTOS, tâches et sémaphores

### Tâche simple
#### En quoi le paramètre TOTAL_HEAP_SIZE a-t-il de l’importance ?
Le paramètre `TOTAL_HEAP_SIZE` détermine la quantité de mémoire disponible pour les allocations dynamiques dans FreeRTOS, si la taille est insuffisante, ces objets ne peuvent pas être créés.

### Sémaphores pour la synchronisation
#### Créez deux tâches avec des priorités différentes (TaskGive et TaskTake), que font-elles ?
- `TaskGive`: Envoie un signal (donne un sémaphore) pour indiquer qu'une ressource est disponible.
- `TaskTake`: Attend le signal (prend le sémaphore) pour accéder à la ressource.

#### Que se passe-t-il si on ajoute 100ms au délai de TaskGive à chaque itération ?
Le délai entre chaque signal envoyé par `TaskGive` augmente progressivement, ce qui ralentit la synchronisation avec `TaskTake` , passé les 1000 ms, la fonction `NVIC_SystemReset();` est activé ce qui reset le STM32, cela agit comme un watchdog.

#### Changez les priorités. Expliquez les changements dans l’affichage.
Si `TaskGive` a une priorité plus élevée que `TaskTake`, elle sera exécutée plus fréquemment, ce qui peut entraîner un comportement plus rapide. Inversement, si `TaskTake` a une priorité plus élevée, elle peut attendre plus longtemps pour recevoir un signal.


### Queues
#### Modifiez TaskGive pour envoyer la valeur du timer dans une queue. Que doit faire TaskTake ?
- `TaskGive` : Envoie la valeur du timer `delay` dans une queue nommé QueuTask avec la fonction`xQueueSend`.
- `TaskTake` : Lit la valeur de la queue à l'aide de `xQueueReceive` et l'affiche via `printf`. Si la réception échoue (timeout), elle peut gérer l'erreur, par exemple en affichant un message ou en déclenchant un reset.

#### Réentrance et exclusion mutuelle
#### Observez attentivement la sortie dans la console. Expliquez d’où vient le problème.

#### print dans le terminal ####
||TP_FreeRTOS_SEHTEL_BEGUIN||// 
Je suis Tache 2 et je m'endors pour 2 ticks 
Je suis Tache 1 et je m'endors pour 2 ticks
Je suis Tache 2 et je m'endors pour 2 ticks 
Je suis Tache 2 et je m'endors pour 2 ticks
Je suis Tache 1 et je m'endors pour 2 ticks 
Je suis Tache 2 et je m'endors pour 2 ticks 
Je suis Tache 2 et je m'endors pour 2 ticks 
Je suis Tache 1 et je m'endors pour 2 ticks 

#### Expliquez d’où vient le problème 
Le problème vient des priorités des tâches et de la configuration des délais. Les deux tâches, `Tache 1` et `Tache 1`, ont des priorités et un délai différents.

#define TASK1_PRIORITY 1
#define TASK2_PRIORITY 2
#define TASK1_DELAY 1
#define TASK2_DELAY 2


Les deux tâches ont des délais très proches. Cela provoque des réveils fréquents et simultanés, mais Tache 2 a une priorité plus élevée sur le processeur.

Conclusion la Tache 2 s'exécute plus souvent que Tache 1 malgré le faite qu celle-ci ai un delay plus faible, ce qui déséquilibre l'affichage dans la console.















 
