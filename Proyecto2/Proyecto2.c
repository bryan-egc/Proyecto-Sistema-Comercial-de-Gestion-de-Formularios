#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CLIENTES   200
#define MAX_PRODUCTOS  200
#define MAX_VENTAS    2000
#define _CRT_SECURE_NO_WARNINGS

   /* ========== Estructuras ========== */
typedef struct {
    int id;
    char nombre[50];
    char email[50];
} Cliente;

typedef struct {
    int id;
    char nombre[50];
    double precio;
    int stock;
} Producto;

typedef struct {
    int id;
    int clienteId;
    int productoId;
    int cantidad;
    double total;
} Venta;

/* ========== Almacenamiento en memoria ========== */
static Cliente clientes[MAX_CLIENTES];
static Producto productos[MAX_PRODUCTOS];
static Venta ventas[MAX_VENTAS];
static int clientesCount = 0, productosCount = 0, ventasCount = 0;
static int nextClienteId = 1, nextProductoId = 1, nextVentaId = 1;

/* ========== Utilidades de entrada ========== */

/* -------------------------------------------------------
   limpiarBuffer
   Limpia el búfer de entrada (stdin) leyendo y
   descartando caracteres hasta el salto de línea.
------------------------------------------------------- */

void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* -------------------------------------------------------
   leerLinea
   Lee una línea de texto con fgets en 'buf' (tam bytes).
   - Si hay '\n' al final, lo elimina.
   - Si hay error/EOF, deja cadena vacía.
   Retorna: (por referencia) el contenido en 'buf'.
------------------------------------------------------- */

void leerLinea(char* buf, int tam) {
    if (fgets(buf, tam, stdin) == NULL) {
        buf[0] = '\0';
        clearerr(stdin);
        return;
    }
    /* quitar salto de linea */
    {
        int len = (int)strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
    }
}

/* -------------------------------------------------------
   leerEnteroRango
   Solicita y valida un entero en [min..max].
   - Reintenta hasta que el usuario ingrese un valor válido.
   - Muestra mensajes de error claros.
   Retorna: entero validado.
------------------------------------------------------- */

int leerEnteroRango(const char* prompt, int min, int max) {
    char linea[128];
    long valor;
    char* fin;
    for (;;) {
        printf("%s", prompt);
        leerLinea(linea, 128);
        if (linea[0] == '\0') continue;
        valor = strtol(linea, &fin, 10);
        if (fin != linea && *fin == '\0' && valor >= min && valor <= max) return (int)valor;
        printf("[Error] Ingrese un entero en rango [%d..%d].\n", min, max);
    }
}

/* -------------------------------------------------------
   leerDoubleMin
   Solicita y valida un double >= minVal.
   - Reintenta hasta dato válido.
   - Mensajes de error consistentes.
   Retorna: double validado.
------------------------------------------------------- */

double leerDoubleMin(const char* prompt, double minVal) {
    char linea[128];
    double valor;
    char* fin;
    for (;;) {
        printf("%s", prompt);
        leerLinea(linea, 128);
        if (linea[0] == '\0') continue;
        valor = strtod(linea, &fin);
        if (fin != linea && *fin == '\0' && valor >= minVal) return valor;
        printf("[Error] Ingrese un numero valido (>= %.2f).\n", minVal);
    }
}

/* -------------------------------------------------------
   leerStringNoVacio
   Solicita un texto y obliga a que no esté vacío.
   - Reintenta si el usuario solo presiona Enter.
   Retorna: (por referencia) texto en 'out'.
------------------------------------------------------- */

void leerStringNoVacio(const char* prompt, char* out, int tamOut) {
    for (;;) {
        int len;
        printf("%s", prompt);
        leerLinea(out, tamOut);
        len = (int)strlen(out);
        if (len > 0) return;
        printf("[Error] No puede estar vacio.\n");
    }
}

/* Variante opcional: leer string que puede quedarse vacio (para conservar valor) */
void leerStringOpcional(const char* prompt, char* out, int tamOut, int* esVacio) {
    printf("%s", prompt);
    leerLinea(out, tamOut);
    if (out[0] == '\0') *esVacio = 1; else *esVacio = 0;
}

/* ========== Buscadores ========== */

/* -------------------------------------------------------
   buscarClientePorId
   Recorre el arreglo 'clientes' y busca por ID.
   Retorna: índice del cliente o -1 si no se encuentra.
------------------------------------------------------- */

int buscarClientePorId(int id) {
    int i;
    for (i = 0; i < clientesCount; i++) if (clientes[i].id == id) return i;
    return -1;
}

/* -------------------------------------------------------
   buscarProductoPorId
   Recorre el arreglo 'productos' y busca por ID.
   Retorna: índice del producto o -1 si no se encuentra.
------------------------------------------------------- */

int buscarProductoPorId(int id) {
    int i;
    for (i = 0; i < productosCount; i++) if (productos[i].id == id) return i;
    return -1;
}

/* ========== Ingresar datos ========== */

/* -------------------------------------------------------
   altaCliente
   Crea un nuevo cliente con ID autoincremental.
   - Valida que haya capacidad.
   - Pide nombre y email (no vacíos).
   Efecto: agrega a 'clientes[]' y aumenta 'clientesCount'.
------------------------------------------------------- */

void altaCliente(void) {
    Cliente c;
    if (clientesCount >= MAX_CLIENTES) { printf("[Error] Capacidad de clientes llena.\n"); return; }
    c.id = nextClienteId++;
    leerStringNoVacio("Nombre del cliente: ", c.nombre, 50);
    leerStringNoVacio("Email del cliente: ", c.email, 50);
    clientes[clientesCount] = c;
    clientesCount++;
    printf("[OK] Cliente creado con ID %d.\n", c.id);
}

/* -------------------------------------------------------
   altaProducto
   Crea un nuevo producto con ID autoincremental.
   - Valida capacidad.
   - Pide nombre, precio >= 0 y stock >= 0.
   Efecto: agrega a 'productos[]' y aumenta 'productosCount'.
------------------------------------------------------- */

void altaProducto(void) {
    Producto p;
    if (productosCount >= MAX_PRODUCTOS) { printf("[Error] Capacidad de productos llena.\n"); return; }
    p.id = nextProductoId++;
    leerStringNoVacio("Nombre del producto: ", p.nombre, 50);
    p.precio = leerDoubleMin("Precio unitario: ", 0.0);
    p.stock = leerEnteroRango("Stock inicial: ", 0, 1000000);
    productos[productosCount] = p;
    productosCount++;
    printf("[OK] Producto creado con ID %d.\n", p.id);
}

/* -------------------------------------------------------
   altaVenta
   Registra una venta:
   - Verifica que existan clientes/productos.
   - Valida IDs y stock disponible.
   - Calcula total = precio * cantidad.
   - Descuenta stock del producto.
   Efecto: agrega a 'ventas[]' y aumenta 'ventasCount'.
------------------------------------------------------- */

void altaVenta(void) {
    int clienteId, productoId, cantidad;
    int ci, pi;
    Venta v;
    if (ventasCount >= MAX_VENTAS) { printf("[Error] Capacidad de ventas llena.\n"); return; }
    if (clientesCount == 0 || productosCount == 0) { printf("[Error] Debe existir al menos 1 cliente y 1 producto.\n"); return; }

    clienteId = leerEnteroRango("ID Cliente: ", 1, 1000000000);
    ci = buscarClientePorId(clienteId);
    if (ci < 0) { printf("[Error] Cliente no existe.\n"); return; }

    productoId = leerEnteroRango("ID Producto: ", 1, 1000000000);
    pi = buscarProductoPorId(productoId);
    if (pi < 0) { printf("[Error] Producto no existe.\n"); return; }

    if (productos[pi].stock <= 0) { printf("[Error] Sin stock disponible.\n"); return; }

    cantidad = leerEnteroRango("Cantidad: ", 1, productos[pi].stock);

    v.id = nextVentaId++;
    v.clienteId = clienteId;
    v.productoId = productoId;
    v.cantidad = cantidad;
    v.total = productos[pi].precio * cantidad;

    productos[pi].stock -= cantidad; /* Descuento de inventario tras confirmar la venta */

    ventas[ventasCount] = v;
    ventasCount++;
    printf("[OK] Venta registrada (ID %d). Total: Q%.2f\n", v.id, v.total);
}

/* -------------------------------------------------------
   menuIngresar
   Submenú de altas:
   1) Cliente   2) Producto   3) Venta   0) Volver
   - Dirige al usuario a la función de alta correspondiente.
------------------------------------------------------- */

void menuIngresar(void) {
    int op;
    for (;;) {
        printf("\n--- Ingresar Datos ---\n");
        printf("1) Nuevo Cliente\n");
        printf("2) Nuevo Producto\n");
        printf("3) Nueva Venta\n");
        printf("0) Volver\n");
        op = leerEnteroRango("Opcion: ", 0, 3);
        if (op == 0) return;
        if (op == 1) altaCliente();
        else if (op == 2) altaProducto();
        else if (op == 3) altaVenta();
    }
}

/* ========== Modificar datos ========== */

/* -------------------------------------------------------
   modificarCliente
   Edita nombre/email de un cliente existente.
   - Busca por ID; si no existe, muestra error.
   - Permite Enter para conservar valores actuales.
   Efecto: actualiza 'clientes[i]'.
------------------------------------------------------- */

void modificarCliente(void) {
    int id, i, vacio;
    char linea[128];
    if (clientesCount == 0) { printf("[Info] No hay clientes.\n"); return; }
    id = leerEnteroRango("ID de cliente a modificar: ", 1, 1000000000);
    i = buscarClientePorId(id);
    if (i < 0) { printf("[Error] Cliente no encontrado.\n"); return; }

    printf("Editando Cliente #%d (Nombre: %s, Email: %s)\n", clientes[i].id, clientes[i].nombre, clientes[i].email);

    leerStringOpcional("Nuevo nombre (Enter para conservar): ", linea, 128, &vacio);
    if (!vacio) {
        /* copia simple */
        strcpy_s(clientes[i].nombre, sizeof(clientes[i].nombre), linea);
    }

    leerStringOpcional("Nuevo email (Enter para conservar): ", linea, 128, &vacio);
    if (!vacio) {
        strcpy_s(clientes[i].email, sizeof(clientes[i].email), linea);
    }

    printf("[OK] Cliente actualizado.\n");
}

/* -------------------------------------------------------
   modificarProducto
   Edita nombre, precio y stock de un producto existente.
   - Busca por ID; valida precio >= 0 y stock >= 0.
   - Enter conserva el valor.
   Efecto: actualiza 'productos[i]'.
------------------------------------------------------- */

void modificarProducto(void) {
    int id, i, vacio;
    char linea[128];
    if (productosCount == 0) { printf("[Info] No hay productos.\n"); return; }
    id = leerEnteroRango("ID de producto a modificar: ", 1, 1000000000);
    i = buscarProductoPorId(id);
    if (i < 0) { printf("[Error] Producto no encontrado.\n"); return; }

    printf("Editando Producto #%d (Nombre: %s, Precio: Q%.2f, Stock: %d)\n", productos[i].id, productos[i].nombre, productos[i].precio, productos[i].stock);

    leerStringOpcional("Nuevo nombre (Enter para conservar): ", linea, 128, &vacio);
    if (!vacio) {
        strcpy(productos[i].nombre, linea);
    }

    printf("Nuevo precio (Enter para conservar): ");
    leerLinea(linea, 128);
    if (linea[0] != '\0') {
        char* fin;
        double v = strtod(linea, &fin);
        if (fin != linea && *fin == '\0' && v >= 0.0) productos[i].precio = v;
        else printf("[Aviso] Precio no valido. Se conserva valor.\n");
    }

    printf("Nuevo stock (Enter para conservar): ");
    leerLinea(linea, 128);
    if (linea[0] != '\0') {
        char* fin;
        long v2 = strtol(linea, &fin, 10);
        if (fin != linea && *fin == '\0' && v2 >= 0) productos[i].stock = (int)v2;
        else printf("[Aviso] Stock no valido. Se conserva valor.\n");
    }

    printf("[OK] Producto actualizado.\n");
}


/* -------------------------------------------------------
   menuModificar
   Submenú de modificación:
   1) Cliente   2) Producto   0) Volver
   - Llama a la función de modificación elegida.
------------------------------------------------------- */

void menuModificar(void) {
    int op;
    for (;;) {
        printf("\n--- Modificar Datos ---\n");
        printf("1) Cliente\n");
        printf("2) Producto\n");
        printf("0) Volver\n");
        op = leerEnteroRango("Opcion: ", 0, 2);
        if (op == 0) return;
        if (op == 1) modificarCliente();
        else if (op == 2) modificarProducto();
    }
}

/* ========== Reportes ========== */

/* -------------------------------------------------------
   listarClientes
   Muestra tabla con: ID, Nombre, Email.
------------------------------------------------------- */

void listarClientes(void) {
    int i;
    printf("\n# Clientes (%d)\n", clientesCount);
    printf("ID | %-20s | %-25s\n", "Nombre", "Email");
    printf("-----------------------------------------------\n");
    for (i = 0; i < clientesCount; i++) {
        printf("%2d | %-20s | %-25s\n", clientes[i].id, clientes[i].nombre, clientes[i].email);
    }
}

/* -------------------------------------------------------
   listarProductos
   Muestra tabla con: ID, Nombre, Precio, Stock.
------------------------------------------------------- */

void listarProductos(void) {
    int i;
    printf("\n# Productos (%d)\n", productosCount);
    printf("ID | %-20s | Precio  | Stock\n", "Nombre");
    printf("-----------------------------------------------\n");
    for (i = 0; i < productosCount; i++) {
        printf("%2d | %-20s | Q%6.2f | %5d\n", productos[i].id, productos[i].nombre, productos[i].precio, productos[i].stock);
    }
}

/* -------------------------------------------------------
   listarVentas
   Muestra tabla con: ID, ClienteID, ProductoID, Cantidad, Total.
------------------------------------------------------- */

void listarVentas(void) {
    int i;
    printf("\n# Ventas (%d)\n", ventasCount);
    printf("ID | Cliente | Producto | Cant | Total\n");
    printf("-----------------------------------------------\n");
    for (i = 0; i < ventasCount; i++) {
        printf("%2d | %7d | %8d | %4d | Q%7.2f\n", ventas[i].id, ventas[i].clienteId, ventas[i].productoId, ventas[i].cantidad, ventas[i].total);
    }
}

/* -------------------------------------------------------
   totalVentas
   Suma los totales de todas las ventas registradas.
   Retorna: monto total vendido (double).
------------------------------------------------------- */

double totalVentas(void) {
    int i; double s = 0.0;
    for (i = 0; i < ventasCount; i++) s += ventas[i].total;
    return s;
}

/* -------------------------------------------------------
   ventasPorCliente
   Agrega ventas por ID de cliente:
   - Cuenta ventas y suma importes.
   Muestra: cantidad y total por cliente.
------------------------------------------------------- */

void ventasPorCliente(void) {
    int id, ci, i, n;
    double s;
    if (clientesCount == 0) { printf("[Info] No hay clientes.\n"); return; }
    id = leerEnteroRango("ID Cliente: ", 1, 1000000000);
    ci = buscarClientePorId(id);
    if (ci < 0) { printf("[Error] Cliente no encontrado.\n"); return; }

    s = 0.0; n = 0;
    for (i = 0; i < ventasCount; i++) if (ventas[i].clienteId == id) { s += ventas[i].total; n++; }
    printf("Cliente #%d (%s): %d venta(s), Total Q%.2f\n", id, clientes[ci].nombre, n, s);
}

/* -------------------------------------------------------
   ventasPorProducto
   Agrega ventas por ID de producto:
   - Suma unidades vendidas e ingresos.
   Muestra: unidades e ingresos por producto.
------------------------------------------------------- */

void ventasPorProducto(void) {
    int id, pi, i, unidades;
    double ingresos;
    if (productosCount == 0) { printf("[Info] No hay productos.\n"); return; }
    id = leerEnteroRango("ID Producto: ", 1, 1000000000);
    pi = buscarProductoPorId(id);
    if (pi < 0) { printf("[Error] Producto no encontrado.\n"); return; }

    unidades = 0; ingresos = 0.0;
    for (i = 0; i < ventasCount; i++) if (ventas[i].productoId == id) { unidades += ventas[i].cantidad; ingresos += ventas[i].total; }
    printf("Producto #%d (%s): %d unidad(es) vendidas, Ingresos Q%.2f\n", id, productos[pi].nombre, unidades, ingresos);
}

/* -------------------------------------------------------
   inventarioValorizado
   Calcula y muestra el valor del inventario:
   - Subtotal = precio * stock por producto.
   - Total = suma de subtotales.
------------------------------------------------------- */

void inventarioValorizado(void) {
    int i; double total = 0.0; double sub;
    printf("\n# Inventario valorizado\n");
    printf("ID | %-20s | Stock | Precio | Subtotal\n", "Nombre");
    printf("-----------------------------------------------------------\n");
    for (i = 0; i < productosCount; i++) {
        sub = productos[i].precio * productos[i].stock;
        total += sub;
        printf("%2d | %-20s | %5d | Q%6.2f | Q%8.2f\n", productos[i].id, productos[i].nombre, productos[i].stock, productos[i].precio, sub);
    }
    printf("Total inventario: Q%.2f\n", total);
}

/* -------------------------------------------------------
   menuReportes
   Submenú de consultas y reportes:
   1) Clientes  2) Productos  3) Ventas
   4) Total ventas  5) Ventas x Cliente
   6) Ventas x Producto  7) Inventario valorizado
   0) Volver
------------------------------------------------------- */

void menuReportes(void) {
    int op;
    for (;;) {
        printf("\n--- Consultar Reportes ---\n");
        printf("1) Listado de Clientes\n");
        printf("2) Listado de Productos\n");
        printf("3) Listado de Ventas\n");
        printf("4) Total de ventas\n");
        printf("5) Ventas por Cliente\n");
        printf("6) Ventas por Producto\n");
        printf("7) Inventario valorizado\n");
        printf("0) Volver\n");
        op = leerEnteroRango("Opcion: ", 0, 7);
        if (op == 0) return;
        if (op == 1) listarClientes();
        else if (op == 2) listarProductos();
        else if (op == 3) listarVentas();
        else if (op == 4) printf("Total vendido: Q%.2f\n", totalVentas());
        else if (op == 5) ventasPorCliente();
        else if (op == 6) ventasPorProducto();
        else if (op == 7) inventarioValorizado();
    }
}

/* ========== Menu Principal ========== */

/* -------------------------------------------------------
   main
   Muestra el menú principal y procesa opciones.
   - Llama a submenús según la elección del usuario.
   - Finaliza si el usuario ingresa 0 (Salir).
   Retorna: 0 al terminar.
------------------------------------------------------- */

int main(void) {
    int op;
    printf("===============================================\n");
    printf("  Sistema Comercial de Gestion de Formularios\n");
    printf("  (Clientes, Productos, Ventas)\n");
    printf("===============================================\n");

    for (;;) {
        printf("\nMenu Principal\n");
        printf("1) Ingresar Datos\n");
        printf("2) Modificar Datos\n");
        printf("3) Consultar Reportes\n");
        printf("0) Salir\n");
        op = leerEnteroRango("Opcion: ", 0, 3);
        if (op == 0) { printf("Saliendo...\n"); break; }
        if (op == 1) menuIngresar();
        else if (op == 2) menuModificar();
        else if (op == 3) menuReportes();
    }
    return 0;
}
