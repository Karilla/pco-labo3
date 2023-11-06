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



## Tests effectués


<!--- Description de chaque test, et information sur le fait qu'il ait passé ou non --->

## Conclusion