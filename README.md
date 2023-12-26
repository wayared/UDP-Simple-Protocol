# UDP-Simple-Protocol
Practica 04 Sistemas Operativos


Diseñar un protocolo simple de transporte que proporcione un servicio confiable al transmitir
un archivo grande. El protocolo debe asegurar que la data sea entregada en orden y sin
duplicados, por lo que será evaluada en una red simulada no confiable.
Su solución deberá seguir los siguientes requerimientos

> - Desarrollar un emisor y receptor en C.
> - Diseñar su solución tomando como protocolo base sólo UDP.
> - El emisor deberá leer un archivo local y usar su protocolo para enviar el recurso al
receptor.
> - Transferir el archivo de manera completa.
> - El receptor deberá recibir el archivo y escribirlo en su directorio local.
> - Tanto emisor y receptor deberán poder finalizar exitosamente.
> - Su solución deberá contemplar la posibilidad de que algunos paquetes sean eliminados,
duplicados y que se entreguen con un cierto retardo.
> - El paquete a transmitir deberá contemplar ciertos campos que sean de ayuda para en
los casos en que tengan que ser retransmitidos.
> - Deberá considerar establecer parámetros para la llamada tanto al emisor como al
receptor Ejm:

> - emisor <host_destino> <puerto_destino> <nombre_archivo>
> - receptor <puerto>


Pruebas de su solución
Para las pruebas deberá contemplar el sgt. escenario:

> - Podrán existir mínimo 2 emisores enviando un archivo grande cada uno a 1 receptor.
> - Deberá contemplar el uso de algún mecanismo de sincronización (mutex, semáforos).
> - Para el ambiente de red no confiable deberán hacer uso del programa netem y
configurarlo para que exista
> - Retardo en la distribución de los paquetes.
> - Perdida de paquetes
> - Duplicidad de paquetes

> - Para la generación del archivo grande podrán utilizar el comando head.
head -c 10485760 <(printf '%.sSistemas Operativos\n' {1..1000000}) > archivo.txt

> - Finalmente para comprobar que los archivos son los mismos podrán utilizar el comando
diff.

