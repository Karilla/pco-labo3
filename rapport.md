# Titre du laboratoire

Auteurs: Benoit Delay, Eva Ray

## Description des fonctionnalités du logiciel

 Le logiciel simule les ventes et achats entre des extracteurs de ressources, des grossistes et des usines. Pour ce faire, nous travaillons avec l'interface ```Seller```, dont les classes ```Extractor```, ```Wholesale``` et ```Factory``` représentant les différentes acteurs de la simulation héritent. 

 Un extracteur est simulé par un thread qui exécute sa méthode ```run()```. Un extracteur va, s'il a les fonds, payer un employé pour extraire la ressource qu'il produit. Un extracteur peut vendre ses ressources à d'autres acteurs de la simulation grâce à sa méthode ```trade(...)```.

 Un grossiste est simulé par un thread qui exécute sa méthode ```run()```. Le grossiste va acheter de manière aléatoire des ressources aux autres acteurs de la simulation. Ces achats sont simulés par la méthode ```buyRessources(...)```. Un grossiste peut revendre ses ressources grâce à sa méthode ```trade(...)```.

 Une usine est simulé par un thread qui exécute sa méthode ```run()```. Si elle a les ressources nécessaires, une usine va constuire la ressource qu'elle produit grâce à sa méthode ```buildItem(...)```. Sinon, elle va essayer d'acheter les ressources nécessaires chez un grossiste en priorisant les ressources qu'elle n'a plus en stock. Cet achat est simulé par la méthode ```orderRessources(...)```. Une usine peut vendre le fruit de sa production aux grossiste grâce à sa méthode ```trade(...)```.

 Ce logiciel simule le comportement de chaque acteur en lançant un thread, il est donc multi-threadé. Par conséquent, il doit assurer une bonne gestion de la concurrence pour les ressources partagées entre plusieurs threads.

La fin de la simulation se fait lorsqu'on ferme la fenêtre de l'application. Les threads doivent s'arrêter correctement et le résultat du calcul du total des fonds est alors affiché.

## Choix d'implémentation

<!--- Comment avez-vous abordé le problème, quels choix avez-vous fait, quelle 
décomposition avez-vous choisie, quelles variables ont dû être protégées, ... ---> 

Comme chaque ```Seller``` est simulé par un thread, nous devons assurer qu'il n'y a pas de problème de concurrence sur les ressources partagées. Les ressources partagées entre les threads sont, pour chaque instance des classes ```Extractor```, ```Wholesale``` ou ```Factory``` les attributs ```stocks``` et ```money```. En effet, on doit s'assurer que si plusieurs threads essaient de mettre à jour ces ressources en parallèle, le résultat final est intègre.

Pour cela, nous avons ajouté un attribut privé ```PcoMutex mutex``` dans la classe abstraite ```Seller``` qu'on va vérouiller avant chaque modification ou certaines lectures des attributs concernés. Nous avions pensé à mettre deux mutex par classe, un pour protéger ```stocks``` et un pour protéger ```money```. Cependant, nous avons observé que ces deux mutex étaient tout le temps vérouillé et dévérouillé en même temps, nous les avons donc fusionnés en un unique mutex. En effet, dans notre simulation d'achat et de vente, les attributs ```stocks``` et ```money``` sont intrinsèquement liés. 

Nous vérouillons le mutex à chaque fois que le contenu des attributs ```stocks``` ou ```money``` est mis à jour ou lorsque nous avons besoin que l'état d'un de ces attributs reste inchangé pendant un certain moment (par exemple, lorsque l'on veut que la condition d'un if reste vraie). Nous avons explicité l'exemple de la méthode  ```Extractor::trade()``` plus bas pour illustrer ces décisions.

Nous avons aussi relâché le mutex autour des fonctions ```PcoThread::usleep(...)``` car ça n'a pas d'incidence sur l'intégrité des ressources partagées et cela nous permet de gagner du temps. 

### Exemple

```cpp
int Extractor::trade(ItemType it, int qty) {

    if(it != resourceExtracted or qty <= 0){
        return 0;
    }

    int price = getMaterialCost() * qty;

    mutex.lock();
    if(stocks[it] < qty ){

        mutex.unlock();
        return 0;
    }

    money += price;
    stocks[it] -= qty;
    mutex.unlock();

    return price;
}
```

Premièrement, nous avons décidé de cinder le ```if``` en deux, ce qui nous permet de ne pas vérouiller le mutex pour rien. Nous avons donc un premier if, avant lequel nous ne vérouillons pas le mutex car il ne concerne que des ressources qui ne sont pas partagées. Nous calculons aussi le prix, qui ne dépend pas non plus de ressources partagées. Nous vérouillons ensuite le mutex avant le deuxième ```if```, car une fois que nous savons que nous avons assez de stock pour faire la transaction, on ne veut pas que cela change. Dans le cas où nous n'avons pas les stocks nécessaires, on n'oublie pas de dévérouiller avant de ```return``` pour éviter un deadlock. On met ensuite ```stocks``` et ```money``` à jour puis on dévérouiller le mutex.    



## Tests effectués

Comme il est difficile de faire des tests unitaires pour ce programme, nous avons vérifé manuellement les points suivants, en se basant sur la fenêtre afficée par l'application.

Pour tester notre programme, nous avons verifié manuellement que 
- Les extracteur minent bien leurs ressources en payant leur employé et en augmentant leurs stocks
- Les extracteur puissent vendre leurs ressources aux grossistes et en diminuant leurs stocks
- Les grossistes payent bien les matières premières et les ajoutent dans leurs stocks
- Les usines achètent les composants dont ils ont besoin aux grossistes et les ajoutent dans leur stock
- Les usines fabriquent bien leur marchandise en payant leurs employés et en augmentant leur stock
- Les usines puissent vendre leur marchandise aux grossistes en diminuant leurs stocks
- La fin de la simulation se passe bien lorsqu'on appuie sur la fenêtre et le résultat final apparaît et est correct

Une exécution typique du programme produit le résultat suivant lorsqu'on l'arrête en appuyant sur la croix:

![](normal_execution.png)

On constate que le résultat des fonds totaux est correct.

## Conclusion