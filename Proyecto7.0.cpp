// Directiva para ignorar la advertencia C4996 sobre funciones inseguras (como localtime) en MSVC
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <fstream>
#include <stack>     // Para la estructura de pila (Stack)
#include <chrono>    // Para obtener la hora actual
#include <ctime>     // Para formatear la hora
#include <iomanip>   // Para manipulación de formato de salida
#include <algorithm> // Para usar swap, remove_if, sort
#include <sstream>   // Para construir encabezados dinámicamente y parsing
#include <cctype>    // Para toupper
#include <numeric>   // Para std::accumulate
#include <functional> // Para std::function

using namespace std;

// ======================= ESTRUCTURAS DE DATOS PRINCIPALES =======================

// Estructura para registrar el movimiento de un camión (existente)
struct RegistroCamion {
	string numeroCamion;
	string conductor;
	string tipoMovimiento; // "ENTRADA" o "SALIDA"
	string hora;
};

// Nueva Estructura para agrupar los datos del Chofer (Opcion 2)
struct Chofer {
	string nombre;
	int id;
	int edad;
	string telefono;
	int codigoPostal;
};

// Nueva Estructura para agrupar los datos de la Unidad de Transporte (Opcion 3)
struct UnidadTransporte {
	int idCamion;
	int placas;
	string color;
	int anio; // Se cambia a int para consistencia, aunque se pedía como string
	string choferAsignado;
	int aniosEnEmpresa;
};

// Nueva Estructura para agrupar los datos de la Empresa (Opcion 1)
struct DatosEmpresa {
	string nombre;
	string auditor;
	string especialista1;
	string especialista2;
	string slogan;
	int anioInicio; // Se añade para el cálculo de años trabajando
};

// NUEVA ESTRUCTURA para manejar registros del Método Burbuja (ingresos_camiones.csv)
struct IngresoRecord {
	string camion;
	string chofer;
	vector<double> ingresos; // Siempre 7 ingresos (Días 1-7)
	double total;
};


// ======================= VARIABLES GLOBALES =======================

// Pila para simular los camiones que están actualmente dentro del patio.
stack<RegistroCamion> camionesEnPatio;
const int MAX_CAMIONES = 5; // Límite de camiones en el patio

// Inicialización de la estructura de datos de la empresa
DatosEmpresa datosDefault = {
	"Padilla y Montano SA de CV",
	"Noemi Torrez Rubio",
	"Dilhery Kenneth Padilla Ortega",
	"Alexandra Hernandez Montano",
	"Padilla y Montano: Liderando el camino.",
	2020 // Ejemplo de año de inicio
};

// ======================= FUNCIONES GENERALES =======================
void limpiarBuffer() {
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void limpiarPantalla() {
	cout << "\nPresione ENTER para continuar...";
	cin.get();
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

// Sobrecarga para limpiar pantalla si sabemos que el buffer esta sucio
void pausaYLimpiar() {
	cout << "\nPresione ENTER para continuar...";
	limpiarBuffer();
	cin.get();
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

int pedirEntero(string mensaje) {
	int valor;
	cout << mensaje;
	if (!(cin >> valor)) {
		cin.clear();
		limpiarBuffer();
		return 0;
	}
	limpiarBuffer();
	return valor;
}

string pedirTexto(string mensaje) {
	string valor;
	cout << mensaje;
	getline(cin, valor);
	return valor;
}

// Calcula la suma de todos los valores de un vector
double sumaValores(const vector<double>& valores) {
	double suma = 0;
	for (double val : valores) suma += val;
	return suma;
}

// ======================= UTILERÍAS DE TIEMPO Y CSV (C4996 fix) =======================

string obtenerHoraActual() {
	auto now = chrono::system_clock::now();
	time_t end_time = chrono::system_clock::to_time_t(now);
	// Nota: localtime() es insegura, pero se mantiene la directiva _CRT_SECURE_NO_WARNINGS
	tm* ltm = localtime(&end_time);
	char buffer[80];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
	return string(buffer);
}

// Se mantiene la función de pila original (no usada en el Case 6)
void guardarMovimientoCSV(const RegistroCamion& registro, bool esNuevoArchivo) {
	ofstream archivo("registro_camiones_pila.csv", ios::app);
	if (esNuevoArchivo) {
		archivo << "Fecha_Hora,Camion,Conductor,Movimiento\n";
	}
	archivo << registro.hora << ","
		<< registro.numeroCamion << ","
		<< registro.conductor << ","
		<< registro.tipoMovimiento << "\n";
	archivo.close();
}


// ======================= LÓGICA DEL SUBMENU 7 (PILA) =======================

void registrarEntrada() {
	cout << "\n======= REGISTRO DE ENTRADA (PUSH) =======\n";
	if (camionesEnPatio.size() >= MAX_CAMIONES) {
		cout << "!!! LIMITE ALCANZADO: El patio ya tiene el maximo de " << MAX_CAMIONES << " camiones." << endl;
		return;
	}

	RegistroCamion nuevoRegistro;
	nuevoRegistro.numeroCamion = pedirTexto("Ingrese el NUMERO del camion: ");
	// Se reutiliza la funcion pedirTexto que usa getline
	nuevoRegistro.conductor = pedirTexto("Ingrese el nombre del CONDUCTOR que ingresa: ");
	nuevoRegistro.tipoMovimiento = "ENTRADA";
	nuevoRegistro.hora = obtenerHoraActual();

	camionesEnPatio.push(nuevoRegistro);

	bool existe = false;
	ifstream archivoLectura("registro_camiones_pila.csv");
	if (archivoLectura.good()) existe = true;
	archivoLectura.close();

	guardarMovimientoCSV(nuevoRegistro, !existe);

	cout << "\n[REGISTRO EXITOSO] Camion: " << nuevoRegistro.numeroCamion
		<< " - Entrada registrada a las: " << nuevoRegistro.hora << endl;
	cout << "Camiones actualmente en patio: " << camionesEnPatio.size() << " de " << MAX_CAMIONES << endl;
}

void registrarSalida() {
	cout << "\n======== REGISTRO DE SALIDA (POP) ========\n";
	if (camionesEnPatio.empty()) {
		cout << "!!! ERROR: El patio de camiones esta vacio. No hay salidas para registrar." << endl;
		return;
	}

	RegistroCamion ultimoCamion = camionesEnPatio.top();

	cout << "[ULTIMO CAMION EN ENTRAR] Camion: " << ultimoCamion.numeroCamion
		<< ", Conductor: " << ultimoCamion.conductor
		<< ", Hora de Entrada: " << ultimoCamion.hora << endl;
	cout << "------------------------------------------------------" << endl;

	string numCamionSalida = pedirTexto("Confirme el NUMERO del camion para salir: ");
	string conductorSalida = pedirTexto("Ingrese el nombre del CONDUCTOR que sale: ");

	if (numCamionSalida != ultimoCamion.numeroCamion) {
		cout << "!!! ADVERTENCIA: El camion ingresado (" << numCamionSalida << ") no coincide con el ultimo en entrar (" << ultimoCamion.numeroCamion << ").\n";
	}

	camionesEnPatio.pop();

	RegistroCamion registroSalida;
	registroSalida.numeroCamion = numCamionSalida;
	registroSalida.conductor = conductorSalida;
	registroSalida.tipoMovimiento = "SALIDA";
	registroSalida.hora = obtenerHoraActual();

	guardarMovimientoCSV(registroSalida, false);

	cout << "\n[REGISTRO EXITOSO] Camion: " << registroSalida.numeroCamion
		<< " - Salida registrada a las: " << registroSalida.hora << endl;
	cout << "Camiones actualmente en patio: " << camionesEnPatio.size() << " de " << MAX_CAMIONES << endl;
}
void mostrarRegistrosTaller(const vector<TallerRecord>& registros) {
	cout << "\n======= DATOS ACTUALES DE COSTOS DE TALLER (" << registros.size() << " Registros) =======\n";
	if (registros.empty()) { cout << "No hay registros guardados para mostrar." << endl; return; }

	// Obtener el máximo de visitas para los encabezados
	size_t maxVisitas = 0;
	for (const auto& r : registros) {
		if (r.costos.size() > maxVisitas) maxVisitas = r.costos.size();
	}

	cout << "----------------------------------------------------------------------------------------------------------------\n";
	cout << "| " << left << setw(10) << "CAMION"
		<< "| " << left << setw(15) << "ENCARGADO"
		<< "| " << left << setw(15) << "TALLER";
	for (size_t i = 0; i < maxVisitas; ++i) {
		cout << "| " << right << setw(7) << "Costo " << (i + 1);
	}
	cout << "| " << right << setw(10) << "TOTAL" << " |\n";
	cout << "----------------------------------------------------------------------------------------------------------------\n";

	cout << fixed << setprecision(2);
	for (const auto& record : registros) {
		cout << "| " << left << setw(10) << record.camion
			<< "| " << left << setw(15) << record.encargado
			<< "| " << left << setw(15) << record.taller;

		for (size_t i = 0; i < maxVisitas; ++i) {
			if (i < record.costos.size()) {
				cout << "| " << right << setw(7) << record.costos[i];
			}
			else {
				cout << "| " << right << setw(7) << "-";
			}
		}
		cout << "| " << right << setw(10) << record.total << " |" << endl;
	}
	cout << "----------------------------------------------------------------------------------------------------------------\n";
}

void mostrarRegistrosEnTabla() {
	ifstream archivo("registro_camiones_pila.csv");
	if (!archivo.is_open()) {
		cout << "Aun no hay registros de movimientos de camiones guardados en el archivo CSV." << endl;
		return;
	}

	cout << "\n======== HISTORIAL DE MOVIMIENTOS DE CAMIONES (TABLA) ========\n";
	cout << "----------------------------------------------------------------------------------" << endl;
	cout << "| " << left << setw(20) << "FECHA Y HORA"
		<< "| " << left << setw(10) << "CAMION"
		<< "| " << left << setw(20) << "CONDUCTOR"
		<< "| " << left << setw(12) << "MOVIMIENTO" << "|" << endl;
	cout << "----------------------------------------------------------------------------------" << endl;

	string linea;
	// Ignorar la línea de encabezados
	getline(archivo, linea);

	while (getline(archivo, linea)) {
		// En un entorno de producción se usaría un parser de CSV más robusto o stringstream
		size_t pos1 = linea.find(',');
		string hora = linea.substr(0, pos1);
		size_t pos2 = linea.find(',', pos1 + 1);
		string camion = linea.substr(pos1 + 1, pos2 - pos1 - 1);
		size_t pos3 = linea.find(',', pos2 + 1);
		string conductor = linea.substr(pos2 + 1, pos3 - pos2 - 1);
		string movimiento = linea.substr(pos3 + 1);

		cout << "| " << left << setw(20) << hora
			<< "| " << left << setw(10) << camion
			<< "| " << left << setw(20) << conductor
			<< "| " << left << setw(12) << movimiento << "|" << endl;
	}
	cout << "----------------------------------------------------------------------------------" << endl;
	archivo.close();
}

void menuCamionesPila() {
	int opcion = 0;
	while (opcion != 4) {
		// system("cls"); // Descomentar para limpiar
		cout << "\n======== REGISTRO DE CAMIONES (PILA: Max " << MAX_CAMIONES << ") ========" << endl;
		cout << "Camiones actualmente en patio: " << camionesEnPatio.size() << " de " << MAX_CAMIONES << endl;
		cout << "-----------------------------------------------------------------" << endl;
		cout << "1. Registrar ENTRADA de camion (PUSH)" << endl;
		cout << "2. Registrar SALIDA de camion (POP)" << endl;
		cout << "3. Mostrar Historial de Registros (Tabla)" << endl;
		cout << "4. Regresar al menu principal" << endl;
		cout << "Seleccione una opcion: ";
		cin >> opcion;
		limpiarBuffer();
		// system("cls"); // Descomentar para limpiar

		switch (opcion) {
		case 1:
			registrarEntrada();
			limpiarPantalla();
			break;
		case 2:
			registrarSalida();
			limpiarPantalla();
			break;
		case 3:
			mostrarRegistrosEnTabla();
			limpiarPantalla();
			break;
		case 4:
			cout << "Regresando al menu principal..." << endl;
			break;
		default:
			cout << "Opcion invalida. Intente de nuevo." << endl;
			limpiarPantalla();
			break;
		}
	}
}


// ======================= IMPLEMENTACIÓN CON ESTRUCTURAS =======================

// Función que reemplaza a nomEmpresa(), nomAuditor(), etc.
int aniosTrabajando(const DatosEmpresa& datos) {
	return 2025 - datos.anioInicio;
}

// Función que utiliza la nueva estructura DatosEmpresa
void mostrarEmpresa(DatosEmpresa& datos) {
	// Pedir año de inicio del desarrollador (aunque el código original pedía "contratación del desarrollador")
	cout << "Ingrese el anio de inicio de operaciones de la empresa: ";
	cin >> datos.anioInicio;
	limpiarBuffer();

	cout << "\n======= DATOS DE LA EMPRESA =======" << endl;
	cout << "Empresa: " << datos.nombre << endl;
	cout << "Auditor: " << datos.auditor << endl;
	cout << "Especialista 1: " << datos.especialista1 << endl;
	cout << "Especialista 2: " << datos.especialista2 << endl;
	// Se utiliza la función y el campo anioInicio de la estructura
	cout << "Anios trabajando (desde " << datos.anioInicio << "): "
		<< aniosTrabajando(datos) << endl;
	cout << "Slogan: " << datos.slogan << endl;
}

// Función que utiliza la nueva estructura Chofer
void mostrarChofer(const Chofer& chofer) {
	cout << "\n--- DATOS DEL CHOFER ---" << endl;
	cout << "Nombre: " << chofer.nombre << endl;
	cout << "ID de chofer: " << chofer.id << endl;
	cout << "Edad: " << chofer.edad << " anios" << endl;
	cout << "Telefono: " << chofer.telefono << endl;
	cout << "Codigo Postal: " << chofer.codigoPostal << endl;
}

// Función que utiliza la nueva estructura UnidadTransporte
void mostrarUnidad(const UnidadTransporte& unidad) {
	cout << "\n--- DATOS DE UNIDAD DE TRANSPORTE ---" << endl;
	cout << "ID de camion: " << unidad.idCamion << endl;
	cout << "Placas: " << unidad.placas << endl;
	cout << "Color: " << unidad.color << endl;
	cout << "Anio: " << unidad.anio << endl;
	cout << "Chofer: " << unidad.choferAsignado << endl;
	cout << "Anios en la empresa: " << unidad.aniosEnEmpresa << endl;
}

// ======================= SUBMENU 5: RECURSIVIDAD (Cálculo de Sueldo) =======================

/**
 * @brief Función recursiva que pide sueldos del mes más reciente al más antiguo.
 * @param mesActual El índice del mes actual (comienza en totalMeses, termina en 1 - el más antiguo).
 * @param totalMeses El número total de meses a calcular (límite superior).
 * @return double El sueldo total acumulado desde el mes más antiguo hasta mesActual.
 */
double calcularSueldoAcumuladoRecursivo(int mesActual, int totalMeses) {
	// Nombres de meses para mostrar
	const vector<string> meses = { "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
								   "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre" };

	// Cálculo para asignar un nombre de mes de forma cíclica (0 a 11)
	int indiceMes = (mesActual - 1) % meses.size();
	string nombreMes = meses[indiceMes];

	double sueldoMes;
	double sueldoAcumulado = 0.0;

	// 1. Pedir el sueldo del mes (del más reciente al más antiguo)
	cout << "Ingrese sueldo para el mes de " << nombreMes << " (Mes #" << mesActual << " de " << totalMeses << "): $";

	// Lectura de sueldo con manejo de error básico
	if (!(cin >> sueldoMes)) {
		cin.clear();
		limpiarBuffer();
		cout << "!!! Advertencia: Entrada invalida. Usando sueldo de $0.00 para este mes.\n";
		sueldoMes = 0.0;
	}

	// 2. Llamada recursiva para el mes anterior
	if (mesActual > 1) {
		// La llamada recursiva devuelve la suma de los sueldos de los meses 1 hasta mesActual-1
		sueldoAcumulado = calcularSueldoAcumuladoRecursivo(mesActual - 1, totalMeses);
	}
	else {
		// Caso Base: mesActual == 1 (el mes más antiguo)
		cout << "\n--- Sueldos ingresados. Mostrando resultados de mas antiguo a mas reciente ---\n";
		sueldoAcumulado = 0.0; // La base de la acumulación es 0
	}

	// 3. Después de regresar (desenrollando la pila): Imprimir y Acumular
	sueldoAcumulado += sueldoMes;

	// 3. (Continuación) Imprimir el sueldo y el acumulado (Orden: Antiguo -> Reciente)
	cout << fixed << setprecision(2);
	cout << "[Mes: " << nombreMes << " (Mes " << mesActual << ")] Sueldo: $" << setw(10) << sueldoMes
		<< " | Acumulado: $" << setw(10) << sueldoAcumulado << endl;

	// 4. Retornar el sueldo acumulado (incluyendo el mes actual)
	return sueldoAcumulado;
}

void menuCalculoSueldoRecursivo() {
	int totalMeses;
	string nombreEmpleado = pedirTexto("Ingrese el nombre del empleado para calcular su sueldo acumulado: ");

	do {
		cout << "Ingrese el numero de meses (N) a calcular (minimo 1, maximo 12): ";
		// Lectura de totalMeses con manejo de error
		if (!(cin >> totalMeses)) {
			cin.clear();
			limpiarBuffer();
			totalMeses = 0; // Para que el bucle repita
		}
		limpiarBuffer();

		if (totalMeses <= 0 || totalMeses > 12) {
			cout << "Numero de meses invalido. Debe ser entre 1 y 12.\n";
		}
	} while (totalMeses <= 0 || totalMeses > 12);

	// system("cls"); // Descomentar para limpiar
	cout << "======= CALCULO DE SUELDO ACUMULADO PARA " << nombreEmpleado << " (" << totalMeses << " MESES) =======\n\n";

	// Inicia la recursión desde el mes más reciente (totalMeses) hasta el más antiguo (1)
	double sueldoFinal = calcularSueldoAcumuladoRecursivo(totalMeses, totalMeses);

	cout << "\n=================================================================\n";
	cout << "RESULTADO FINAL: El sueldo acumulado total en " << totalMeses
		<< " meses para " << nombreEmpleado << " es: $" << fixed << setprecision(2) << sueldoFinal << endl;
	cout << "=================================================================\n";
}


// ======================= SUBMENU 6: METODOS DE ORDENAMIENTO Y LISTAS =======================

// --------------------------- UTILIDADES CSV Y LISTAS (BURBUJA) ---------------------------

const string NOMBRE_ARCHIVO_INGRESOS = "ingresos_camiones.csv";

/**
 * @brief Carga todos los registros de ingresos desde el CSV.
 */
vector<IngresoRecord> cargarIngresosCSV() {
	vector<IngresoRecord> registros;
	ifstream archivo(NOMBRE_ARCHIVO_INGRESOS);
	if (!archivo.is_open()) return registros; // Retorna vector vacío si no existe

	string linea;
	getline(archivo, linea); // Ignorar encabezados

	while (getline(archivo, linea)) {
		stringstream ss(linea);
		string segmento;
		IngresoRecord record;

		// 1. Camion
		getline(ss, segmento, ',');
		record.camion = segmento;
		// 2. Chofer
		getline(ss, segmento, ',');
		record.chofer = segmento;

		// 3. Ingresos (7 días)
		record.ingresos.clear();
		for (int i = 0; i < 7; ++i) {
			if (getline(ss, segmento, ',')) {
				try {
					record.ingresos.push_back(stod(segmento));
				}
				catch (...) {
					record.ingresos.push_back(0.0); // Manejo de error
				}
			}
			else break; // Si no hay más segmentos
		}

		// 4. Total (El último segmento después de los 7 ingresos)
		if (getline(ss, segmento, ',')) {
			try {
				record.total = stod(segmento);
			}
			catch (...) {
				record.total = sumaValores(record.ingresos); // Recalcular si falla
			}
		}
		else {
			record.total = sumaValores(record.ingresos);
		}
		registros.push_back(record);
	}
	archivo.close();
	return registros;
}
// ======================= ESTRUCTURAS DE DATOS PARA LISTAS (CASE 6) =======================

// 1. Método Burbuja (ingresos_camiones.csv)
struct IngresoRecord {
	string camion;
	string chofer;
	vector<double> ingresos; // Días 1-7
	double total;
};
const string NOMBRE_ARCHIVO_INGRESOS = "ingresos_camiones.csv";

// 2. Método Selección (taller_camiones.csv)
struct TallerRecord {
	string camion;
	string encargado;
	string taller;
	vector<double> costos; // Visitas
	double total;
};
const string NOMBRE_ARCHIVO_TALLER = "taller_camiones.csv";

// 3. Método Inserción (kilometraje_camiones.csv)
struct KilometrajeRecord {
	string camion;
	string chofer;
	vector<double> km; // Días
	double total;
};
const string NOMBRE_ARCHIVO_KM = "kilometraje_camiones.csv";

// ======================= CRUD COMÚN (CARGAR/GUARDAR/ELIMINAR) =======================

// --------------------------- INGRESOS (BURBUJA) ---------------------------

vector<IngresoRecord> cargarIngresosCSV() {
	vector<IngresoRecord> registros;
	ifstream archivo(NOMBRE_ARCHIVO_INGRESOS);
	if (!archivo.is_open()) return registros;
	string linea;
	getline(archivo, linea); // Ignorar encabezados

	while (getline(archivo, linea)) {
		stringstream ss(linea);
		string segmento;
		IngresoRecord record;

		getline(ss, record.camion, ',');
		getline(ss, record.chofer, ',');

		record.ingresos.clear();
		for (int i = 0; i < 7; ++i) {
			if (getline(ss, segmento, ',')) {
				try {
					record.ingresos.push_back(stod(segmento));
				}
				catch (...) { record.ingresos.push_back(0.0); }
			}
			else break;
		}

		if (getline(ss, segmento, ',')) {
			try { record.total = stod(segmento); }
			catch (...) { record.total = sumaValores(record.ingresos); }
		}
		else { record.total = sumaValores(record.ingresos); }
		registros.push_back(record);
	}
	archivo.close();
	return registros;
}
void guardarIngresosCSV_Sobrescribir(const vector<IngresoRecord>& registros) {
	ofstream archivo(NOMBRE_ARCHIVO_INGRESOS, ios::out);
	if (archivo.is_open()) {
		archivo << "Camion,Chofer,Dia_1,Dia_2,Dia_3,Dia_4,Dia_5,Dia_6,Dia_7,Total\n";
		archivo << fixed << setprecision(2);
		for (const auto& record : registros) {
			archivo << record.camion << "," << record.chofer;
			for (double val : record.ingresos) archivo << "," << val;
			archivo << "," << record.total << "\n";
		}
		archivo.close();
		cout << "[CSV ACTUALIZADO] La tabla de registros ha sido sobrescrita en '" << NOMBRE_ARCHIVO_INGRESOS << "'.\n";
	}
	else {
		cout << "!!! ERROR: No se pudo abrir el archivo para sobrescribir los datos.\n";
	}
}
void mostrarRegistrosIngresos(const vector<IngresoRecord>& registros) {
	cout << "\n======= DATOS ACTUALES DE INGRESOS (" << registros.size() << " Registros) =======\n";
	if (registros.empty()) { cout << "No hay registros guardados para mostrar." << endl; return; }

	cout << "----------------------------------------------------------------------------------------------------------------\n";
	cout << "| " << left << setw(10) << "CAMION"
		<< "| " << left << setw(15) << "CHOFER"
		<< "| " << right << setw(7) << "Dia 1"
		<< "| " << right << setw(7) << "Dia 2"
		<< "| " << right << setw(7) << "Dia 3"
		<< "| " << right << setw(7) << "Dia 4"
		<< "| " << right << setw(7) << "Dia 5"
		<< "| " << right << setw(7) << "Dia 6"
		<< "| " << right << setw(7) << "Dia 7"
		<< "| " << right << setw(10) << "TOTAL" << " |\n";
	cout << "----------------------------------------------------------------------------------------------------------------\n";

	cout << fixed << setprecision(2);
	for (const auto& record : registros) {
		cout << "| " << left << setw(10) << record.camion
			<< "| " << left << setw(15) << record.chofer;

		for (double ingreso : record.ingresos) {
			cout << "| " << right << setw(7) << ingreso;
		}
		cout << "| " << right << setw(10) << record.total << " |" << endl;
	}
	cout << "----------------------------------------------------------------------------------------------------------------\n";
}

void eliminarRegistroIngreso(vector<IngresoRecord>& registros) {
	cout << "\n======= ELIMINAR REGISTRO DE INGRESO =======\n";
	string camion = pedirTexto("Ingrese el numero de CAMION a eliminar: ");
	string chofer = pedirTexto("Ingrese el nombre del CHOFER del registro a eliminar: ");
	size_t tam_antes = registros.size();

	registros.erase(
		remove_if(registros.begin(), registros.end(),
			[camion, chofer](const IngresoRecord& r) {
				return r.camion == camion && r.chofer == chofer;
			}),
		registros.end()
	);
	size_t tam_despues = registros.size();

	if (tam_despues < tam_antes) {
		cout << "\n[ELIMINACION EXITOSA] Se eliminaron " << (tam_antes - tam_despues) << " registros.\n";
		guardarIngresosCSV_Sobrescribir(registros);
	}
	else {
		cout << "\n[ADVERTENCIA] No se encontro ningun registro con Camion: " << camion << " y Chofer: " << chofer << ".\n";
	}
}
// Adaptador para Metodo Selección (Taller)
void menuListaTallerCompleto(const string& metodoUsado) {
	bool regresar = menuListaGenerico<TallerRecord>(
		metodoUsado,
		cargarTallerCSV,
		mostrarRegistrosTaller,
		eliminarRegistroTaller,
		actualizarRegistroTaller,
		ordenarRegistrosTaller
	);
	if (regresar) {
		metodoSeleccion();
	}
}
// Adaptador para Metodo Inserción (Kilometraje)
void menuListaKilometrajeCompleto(const string& metodoUsado) {
	bool regresar = menuListaGenerico<KilometrajeRecord>(
		metodoUsado,
		cargarKmCSV,
		mostrarRegistrosKm,
		eliminarRegistroKm,
		actualizarRegistroKm,
		ordenarRegistrosKm
	);
	if (regresar) {
		metodoInsercion();
	}
}
// Adaptador para Metodo Burbuja (Ingresos)
void menuListaIngresosCompleto(const string& metodoUsado) {
	// Las lambdas envuelven las funciones de actualización y ordenamiento específicas.
	// La función cargar() se llama automáticamente dentro del bucle del menú genérico.
	bool regresar = menuListaGenerico<IngresoRecord>(
		metodoUsado,
		cargarIngresosCSV,
		mostrarRegistrosIngresos,
		eliminarRegistroIngreso,
		actualizarRegistroIngreso,
		ordenarRegistrosIngreso
	);
	// Si regresa es true, volvemos a llamar a metodoBurbuja, si no, salimos y volvemos al menu principal.
	if (regresar) {
		metodoBurbuja(); // Llama a la función principal para agregar un nuevo registro
	}
}
// ======================= MENUS DE LISTAS COMPLETOS =======================

/**
 * @brief Menú de listas genérico adaptado a cada tipo de registro.
 * La lógica de carga, guardado, eliminación y visualización debe ser inyectada.
 * La lógica de Búsqueda, Actualización y Ordenamiento es específica.
 * @return true si se debe regresar al menú de ordenamiento (para agregar un nuevo registro), false para salir.
 */
template<typename T, typename C, typename S, typename E, typename U, typename O>
bool menuListaGenerico(const string& metodoUsado, C cargar, S mostrar, E eliminar, U actualizar, O ordenar) {
	int opcion = 0;
	bool regresarMenuOrdenamiento = false;

	while (opcion != 9) {
		vector<T> registros = cargar();

		cout << "\n======== MENU DE LISTAS DE REGISTROS (" << metodoUsado << ") ========\n";
		cout << "Registros actuales: " << registros.size() << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << "1. Mostrar todos los registros actuales" << endl;
		cout << "2. Agregar un nuevo registro (Regresar al Menu Ordenamiento)" << endl;
		cout << "3. Eliminar un registro" << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << "4. Búsqueda: Encontrar un elemento en la lista" << endl;
		cout << "5. Actualización: Modificar el valor de un elemento en la lista" << endl;
		cout << "6. Ordenamiento: Organizar los elementos en un orden específico" << endl;
		cout << "7. Tamaño: Obtener cuántos elementos tiene la lista" << endl;
		cout << "8. Verificar si está vacía: Comprobar si la lista no contiene ningún elemento" << endl;
		cout << "9. Regresar al Menu Principal" << endl;
		cout << "Seleccione una opcion: ";
		cin >> opcion;
		limpiarBuffer();

		// vector<T> registros_operacion = cargar(); // Recargar para operaciones, aunque ya se hizo arriba

		switch (opcion) {
		case 1:
			mostrar(registros);
			limpiarPantalla();
			break;
		case 2:
			regresarMenuOrdenamiento = true;
			opcion = 9; // Salir del bucle
			break;
		case 3:
			eliminar(registros);
			limpiarPantalla();
			break;
		case 4:
			buscarRegistro<T>(registros, metodoUsado);
			limpiarPantalla();
			break;
		case 5:
			actualizar(registros);
			limpiarPantalla();
			break;
		case 6:
			ordenar(registros); // Muestra la lista ordenada dentro de la función
			limpiarPantalla();
			break;
		case 7:
			cout << "\n[TAMAÑO] La lista contiene " << registros.size() << " elementos.\n";
			limpiarPantalla();
			break;
		case 8:
			if (registros.empty()) {
				cout << "\n[ESTADO] La lista ESTA vacia.\n";
			}
			else {
				cout << "\n[ESTADO] La lista NO esta vacia. Contiene " << registros.size() << " elementos.\n";
			}
			limpiarPantalla();
			break;
		case 9:
			cout << "Regresando al Menu Principal...\n";
			break;
		default:
			cout << "Opcion invalida. Intente de nuevo." << endl;
			limpiarPantalla();
			break;
		}
	}
	return regresarMenuOrdenamiento;
}
void ordenarRegistrosKm(vector<KilometrajeRecord>& registros) {
	cout << "\n======= ORDENAR REGISTROS DE KILOMETRAJE =======\n";
	cout << "Ordenar por: 1. Camion (Texto) | 2. Chofer (Texto) | 3. Total (Numerico): ";
	int campo = pedirEntero("");
	cout << "¿Orden Ascendente (1) o Descendente (2)?: ";
	int orden = pedirEntero("");
	bool asc = (orden == 1);

	if (campo == 1) { // Camion
		sort(registros.begin(), registros.end(), [asc](const KilometrajeRecord& a, const KilometrajeRecord& b) {
			return asc ? (a.camion < b.camion) : (a.camion > b.camion);
			});
	}
	else if (campo == 2) { // Chofer
		sort(registros.begin(), registros.end(), [asc](const KilometrajeRecord& a, const KilometrajeRecord& b) {
			return asc ? (a.chofer < b.chofer) : (a.chofer > b.chofer);
			});
	}
	else if (campo == 3) { // Total
		sort(registros.begin(), registros.end(), [asc](const KilometrajeRecord& a, const KilometrajeRecord& b) {
			return asc ? (a.total < b.total) : (a.total > b.total);
			});
	}
	else {
		cout << "[ADVERTENCIA] Opcion de campo invalida. No se realizo el ordenamiento.\n";
		return;
	}
	cout << "[ORDENAMIENTO] Lista ordenada por campo seleccionado " << (asc ? "ASCENDENTE" : "DESCENDENTE") << " (Temporalmente).\n";
	mostrarRegistrosKm(registros);
}
// --- Operaciones de Ordenamiento ---
void ordenarRegistrosIngreso(vector<IngresoRecord>& registros) {
	cout << "\n======= ORDENAR REGISTROS DE INGRESO =======\n";
	cout << "Ordenar por: 1. Camion (Texto) | 2. Chofer (Texto) | 3. Total (Numerico): ";
	int campo = pedirEntero("");
	cout << "¿Orden Ascendente (1) o Descendente (2)?: ";
	int orden = pedirEntero("");
	bool asc = (orden == 1);

	if (campo == 1) { // Camion
		sort(registros.begin(), registros.end(), [asc](const IngresoRecord& a, const IngresoRecord& b) {
			return asc ? (a.camion < b.camion) : (a.camion > b.camion);
			});
		cout << "[ORDENAMIENTO] Lista ordenada por Camion " << (asc ? "ASCENDENTE" : "DESCENDENTE") << " (Temporalmente).\n";
	}
	else if (campo == 2) { // Chofer
		sort(registros.begin(), registros.end(), [asc](const IngresoRecord& a, const IngresoRecord& b) {
			return asc ? (a.chofer < b.chofer) : (a.chofer > b.chofer);
			});
		cout << "[ORDENAMIENTO] Lista ordenada por Chofer " << (asc ? "ASCENDENTE" : "DESCENDENTE") << " (Temporalmente).\n";
	}
	else if (campo == 3) { // Total
		sort(registros.begin(), registros.end(), [asc](const IngresoRecord& a, const IngresoRecord& b) {
			return asc ? (a.total < b.total) : (a.total > b.total);
			});
		cout << "[ORDENAMIENTO] Lista ordenada por Total " << (asc ? "ASCENDENTE" : "DESCENDENTE") << " (Temporalmente).\n";
	}
	else {
		cout << "[ADVERTENCIA] Opcion de campo invalida. No se realizo el ordenamiento.\n";
	}
	mostrarRegistrosIngresos(registros); // Mostrar resultado
}

void ordenarRegistrosTaller(vector<TallerRecord>& registros) {
	cout << "\n======= ORDENAR REGISTROS DE TALLER =======\n";
	cout << "Ordenar por: 1. Camion (Texto) | 2. Taller (Texto) | 3. Total (Numerico): ";
	int campo = pedirEntero("");
	cout << "¿Orden Ascendente (1) o Descendente (2)?: ";
	int orden = pedirEntero("");
	bool asc = (orden == 1);

	if (campo == 1) { // Camion
		sort(registros.begin(), registros.end(), [asc](const TallerRecord& a, const TallerRecord& b) {
			return asc ? (a.camion < b.camion) : (a.camion > b.camion);
			});
	}
	else if (campo == 2) { // Taller
		sort(registros.begin(), registros.end(), [asc](const TallerRecord& a, const TallerRecord& b) {
			return asc ? (a.taller < b.taller) : (a.taller > b.taller);
			});
	}
	else if (campo == 3) { // Total
		sort(registros.begin(), registros.end(), [asc](const TallerRecord& a, const TallerRecord& b) {
			return asc ? (a.total < b.total) : (a.total > b.total);
			});
	}
	else {
		cout << "[ADVERTENCIA] Opcion de campo invalida. No se realizo el ordenamiento.\n";
		return;
	}
	cout << "[ORDENAMIENTO] Lista ordenada por campo seleccionado " << (asc ? "ASCENDENTE" : "DESCENDENTE") << " (Temporalmente).\n";
	mostrarRegistrosTaller(registros);
}

// --------------------------- KILOMETRAJE (INSERCIÓN) ---------------------------

vector<KilometrajeRecord> cargarKmCSV() {
	vector<KilometrajeRecord> registros;
	ifstream archivo(NOMBRE_ARCHIVO_KM);
	if (!archivo.is_open()) return registros;
	string linea;
	getline(archivo, linea); // Ignorar encabezados

	while (getline(archivo, linea)) {
		stringstream ss(linea);
		string segmento;
		KilometrajeRecord record;

		getline(ss, record.camion, ',');
		getline(ss, record.chofer, ',');

		record.km.clear();
		while (getline(ss, segmento, ',')) {
			try {
				if (ss.peek() == EOF || ss.peek() == '\n') { // Último elemento es el total
					record.total = stod(segmento);
					break;
				}
				record.km.push_back(stod(segmento));
			}
			catch (...) { record.km.push_back(0.0); }
		}
		registros.push_back(record);
	}
	archivo.close();
	return registros;
}
void guardarKmCSV_Sobrescribir(const vector<KilometrajeRecord>& registros) {
	ofstream archivo(NOMBRE_ARCHIVO_KM, ios::out);
	if (archivo.is_open()) {
		// Encabezados dinámicos
		archivo << "Camion,Chofer";
		if (!registros.empty()) {
			for (size_t i = 0; i < registros[0].km.size(); ++i) {
				archivo << ",Km_Dia_" << (i + 1);
			}
		}
		else {
			archivo << ",Km_Dia_1"; // Poner al menos uno si está vacío
		}
		archivo << ",Total\n";

		archivo << fixed << setprecision(2);
		for (const auto& record : registros) {
			archivo << record.camion << "," << record.chofer;
			for (double val : record.km) archivo << "," << val;
			archivo << "," << record.total << "\n";
		}
		archivo.close();
		cout << "[CSV ACTUALIZADO] La tabla de registros ha sido sobrescrita en '" << NOMBRE_ARCHIVO_KM << "'.\n";
	}
	else {
		cout << "!!! ERROR: No se pudo abrir el archivo para sobrescribir los datos.\n";
	}
}
void eliminarRegistroTaller(vector<TallerRecord>& registros) {
	cout << "\n======= ELIMINAR REGISTRO DE TALLER =======\n";
	string camion = pedirTexto("Ingrese el numero de CAMION a eliminar: ");
	string taller = pedirTexto("Ingrese el nombre del TALLER del registro a eliminar: ");
	size_t tam_antes = registros.size();

	registros.erase(
		remove_if(registros.begin(), registros.end(),
			[camion, taller](const TallerRecord& r) {
				return r.camion == camion && r.taller == taller;
			}),
		registros.end()
	);
	size_t tam_despues = registros.size();

	if (tam_despues < tam_antes) {
		cout << "\n[ELIMINACION EXITOSA] Se eliminaron " << (tam_antes - tam_despues) << " registros.\n";
		guardarTallerCSV_Sobrescribir(registros);
	}
	else {
		cout << "\n[ADVERTENCIA] No se encontro ningun registro con Camion: " << camion << " y Taller: " << taller << ".\n";
	}
}

void mostrarRegistrosKm(const vector<KilometrajeRecord>& registros) {
	cout << "\n======= DATOS ACTUALES DE KILOMETRAJE (" << registros.size() << " Registros) =======\n";
	if (registros.empty()) { cout << "No hay registros guardados para mostrar." << endl; return; }

	size_t maxDias = 0;
	for (const auto& r : registros) {
		if (r.km.size() > maxDias) maxDias = r.km.size();
	}

	cout << "----------------------------------------------------------------------------------------------------------------\n";
	cout << "| " << left << setw(10) << "CAMION"
		<< "| " << left << setw(15) << "CHOFER";
	for (size_t i = 0; i < maxDias; ++i) {
		cout << "| " << right << setw(7) << "Dia " << (i + 1);
	}
	cout << "| " << right << setw(10) << "TOTAL" << " |\n";
	cout << "----------------------------------------------------------------------------------------------------------------\n";

	cout << fixed << setprecision(2);
	for (const auto& record : registros) {
		cout << "| " << left << setw(10) << record.camion
			<< "| " << left << setw(15) << record.chofer;

		for (size_t i = 0; i < maxDias; ++i) {
			if (i < record.km.size()) {
				cout << "| " << right << setw(7) << record.km[i];
			}
			else {
				cout << "| " << right << setw(7) << "-";
			}
		}
		cout << "| " << right << setw(10) << record.total << " |" << endl;
	}
	cout << "----------------------------------------------------------------------------------------------------------------\n";
}

void eliminarRegistroKm(vector<KilometrajeRecord>& registros) {
	cout << "\n======= ELIMINAR REGISTRO DE KILOMETRAJE =======\n";
	string camion = pedirTexto("Ingrese el numero de CAMION a eliminar: ");
	string chofer = pedirTexto("Ingrese el nombre del CHOFER del registro a eliminar: ");
	size_t tam_antes = registros.size();

	registros.erase(
		remove_if(registros.begin(), registros.end(),
			[camion, chofer](const KilometrajeRecord& r) {
				return r.camion == camion && r.chofer == chofer;
			}),
		registros.end()
	);
	size_t tam_despues = registros.size();

	if (tam_despues < tam_antes) {
		cout << "\n[ELIMINACION EXITOSA] Se eliminaron " << (tam_antes - tam_despues) << " registros.\n";
		guardarKmCSV_Sobrescribir(registros);
	}
	else {
		cout << "\n[ADVERTENCIA] No se encontro ningun registro con Camion: " << camion << " y Chofer: " << chofer << ".\n";
	}
}


// ======================= OPERACIONES AVANZADAS DE LISTA =======================

// --- Operaciones de Búsqueda ---
template<typename T>
void buscarRegistro(const vector<T>& registros, const string& tipoRegistro) {
	if (registros.empty()) { cout << "La lista esta vacia. No se puede buscar." << endl; return; }

	cout << "\n======= BUSQUEDA DE REGISTRO (" << tipoRegistro << ") =======\n";
	string busqueda = pedirTexto("Ingrese el valor de busqueda (Camion o Chofer/Encargado): ");
	int count = 0;

	for (const auto& r : registros) {
		bool encontrado = false;
		if constexpr (is_same_v<T, IngresoRecord>) {
			if (r.camion == busqueda || r.chofer == busqueda) encontrado = true;
		}
		else if constexpr (is_same_v<T, TallerRecord>) {
			if (r.camion == busqueda || r.encargado == busqueda || r.taller == busqueda) encontrado = true;
		}
		else if constexpr (is_same_v<T, KilometrajeRecord>) {
			if (r.camion == busqueda || r.chofer == busqueda) encontrado = true;
		}

		if (encontrado) {
			cout << "\n[ENCONTRADO #" << (++count) << "]\n";
			if constexpr (is_same_v<T, IngresoRecord>) {
				cout << "  Camion: " << r.camion << ", Chofer: " << r.chofer << ", Total Ingresos: " << fixed << setprecision(2) << r.total << endl;
			}
			else if constexpr (is_same_v<T, TallerRecord>) {
				cout << "  Camion: " << r.camion << ", Encargado: " << r.encargado << ", Taller: " << r.taller << ", Total Costos: " << fixed << setprecision(2) << r.total << endl;
			}
			else if constexpr (is_same_v<T, KilometrajeRecord>) {
				cout << "  Camion: " << r.camion << ", Chofer: " << r.chofer << ", Total KM: " << fixed << setprecision(2) << r.total << endl;
			}
		}
	}

	if (count == 0) {
		cout << "[NO ENCONTRADO] No se encontraron registros que coincidan con la busqueda: '" << busqueda << "'.\n";
	}
	else {
		cout << "\n[RESUMEN] Se encontraron " << count << " registros coincidentes.\n";
	}
}

// --- Operaciones de Actualización ---
void actualizarRegistroIngreso(vector<IngresoRecord>& registros) {
	cout << "\n======= ACTUALIZAR REGISTRO DE INGRESO =======\n";
	string camion = pedirTexto("Ingrese el numero de CAMION a actualizar: ");
	string chofer = pedirTexto("Ingrese el nombre del CHOFER a actualizar: ");

	auto it = find_if(registros.begin(), registros.end(),
		[camion, chofer](const IngresoRecord& r) {
			return r.camion == camion && r.chofer == chofer;
		});

	if (it != registros.end()) {
		cout << "[ENCONTRADO] Actualizando registro de Camion: " << camion << ", Chofer: " << chofer << endl;
		IngresoRecord& r = *it;

		for (size_t i = 0; i < r.ingresos.size(); ++i) {
			r.ingresos[i] = pedirDouble("  Nuevo ingreso para Dia " + to_string(i + 1) + " (Anterior: " + to_string(r.ingresos[i]) + "): ");
		}

		r.total = sumaValores(r.ingresos);
		guardarIngresosCSV_Sobrescribir(registros);
		cout << "[ACTUALIZACION EXITOSA] Total recalculado: " << fixed << setprecision(2) << r.total << endl;
	}
	else {
		cout << "[ADVERTENCIA] No se encontro el registro para actualizar.\n";
	}
}

void actualizarRegistroTaller(vector<TallerRecord>& registros) {
	cout << "\n======= ACTUALIZAR REGISTRO DE TALLER =======\n";
	string camion = pedirTexto("Ingrese el numero de CAMION a actualizar: ");
	string taller = pedirTexto("Ingrese el nombre del TALLER a actualizar: ");

	auto it = find_if(registros.begin(), registros.end(),
		[camion, taller](const TallerRecord& r) {
			return r.camion == camion && r.taller == taller;
		});

	if (it != registros.end()) {
		cout << "[ENCONTRADO] Actualizando registro de Camion: " << camion << ", Taller: " << taller << endl;
		TallerRecord& r = *it;

		for (size_t i = 0; i < r.costos.size(); ++i) {
			r.costos[i] = pedirDouble("  Nuevo costo para Visita " + to_string(i + 1) + " (Anterior: " + to_string(r.costos[i]) + "): ");
		}
		r.encargado = pedirTexto("  Nuevo nombre del Encargado (Anterior: " + r.encargado + "): ");

		r.total = sumaValores(r.costos);
		guardarTallerCSV_Sobrescribir(registros);
		cout << "[ACTUALIZACION EXITOSA] Total recalculado: " << fixed << setprecision(2) << r.total << endl;
	}
	else {
		cout << "[ADVERTENCIA] No se encontro el registro para actualizar.\n";
	}
}

void actualizarRegistroKm(vector<KilometrajeRecord>& registros) {
	cout << "\n======= ACTUALIZAR REGISTRO DE KILOMETRAJE =======\n";
	string camion = pedirTexto("Ingrese el numero de CAMION a actualizar: ");
	string chofer = pedirTexto("Ingrese el nombre del CHOFER a actualizar: ");

	auto it = find_if(registros.begin(), registros.end(),
		[camion, chofer](const KilometrajeRecord& r) {
			return r.camion == camion && r.chofer == chofer;
		});

	if (it != registros.end()) {
		cout << "[ENCONTRADO] Actualizando registro de Camion: " << camion << ", Chofer: " << chofer << endl;
		KilometrajeRecord& r = *it;

		for (size_t i = 0; i < r.km.size(); ++i) {
			r.km[i] = pedirDouble("  Nuevo KM para Dia " + to_string(i + 1) + " (Anterior: " + to_string(r.km[i]) + "): ");
		}

		r.total = sumaValores(r.km);
		guardarKmCSV_Sobrescribir(registros);
		cout << "[ACTUALIZACION EXITOSA] Total recalculado: " << fixed << setprecision(2) << r.total << endl;
	}
	else {
		cout << "[ADVERTENCIA] No se encontro el registro para actualizar.\n";
	}
}
// ---------- MÉTODO BURBUJA (Adaptado para List Menu) ----------
void burbujaDesc(vector<double>& ingresos) {
	for (size_t i = 0; i < ingresos.size() - 1; i++) {
		for (size_t j = 0; j < ingresos.size() - i - 1; j++) {
			if (ingresos[j] < ingresos[j + 1]) {
				swap(ingresos[j], ingresos[j + 1]);
			}
		}
	}
}
// ======================= METODOS DE ORDENAMIENTO (CASE 6) =======================

string obtenerNombreDia(int indice, const vector<string>& diasSemana, bool esSemanaActual) {
	if (esSemanaActual && indice >= 0 && indice < diasSemana.size()) {
		return diasSemana[indice];
	}
	return "Dia " + to_string(indice + 1);
}
// Se mantiene la funcion de guardar original para APPENDAR el registro (solo para Burbuja)
void guardarIngresosCSV_Append(string camion, string chofer, const vector<double>& ingresos, double total) {
	const string nombreArchivo = NOMBRE_ARCHIVO_INGRESOS;
	bool archivoVacio = false;
	ifstream verificar(nombreArchivo);
	if (verificar.is_open()) {
		verificar.seekg(0, ios::end);
		if (verificar.tellg() == 0) archivoVacio = true;
		verificar.close();
	}
	else {
		archivoVacio = true;
	}

	ofstream archivo(nombreArchivo, ios::app);
	if (archivo.is_open()) {
		if (archivoVacio) {
			archivo << "Camion,Chofer,Dia_1,Dia_2,Dia_3,Dia_4,Dia_5,Dia_6,Dia_7,Total\n";
		}
		archivo << fixed << setprecision(2);
		archivo << camion << "," << chofer;
		for (double val : ingresos) archivo << "," << val;
		archivo << "," << total << "\n";
		archivo.close();
	}
	else {
		cout << "No se pudo abrir el archivo para guardar los datos.\n";
	}
}

void metodoBurbuja() {
	string camion, chofer;
	cout << "\n======= METODO BURBUJA: REGISTRO DE INGRESOS (ORDENAMIENTO DESCENDENTE) =======\n";
	cout << "Ingrese el numero de camion: ";
	cin >> camion;
	limpiarBuffer();
	cout << "Ingrese el nombre del chofer: ";
	getline(cin, chofer);

	vector<double> ingresos(7);
	char resp;
	cout << "¿Los ingresos corresponden a los 7 dias de una semana actual (Lunes a Domingo)? (S/N): ";
	cin >> resp;
	limpiarBuffer();

	bool esSemanaActual = (toupper(resp) == 'S');
	vector<string> diasSemana = { "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo" };

	cout << "\nIngrese los ingresos por cada dia de la semana:\n";
	for (size_t i = 0; i < ingresos.size(); i++) {
		ingresos[i] = pedirDouble(obtenerNombreDia(i, diasSemana, esSemanaActual) + " (" + to_string(i + 1) + "): ");
	}

	double total = sumaValores(ingresos);
	burbujaDesc(ingresos); // Ordenar

	cout << "\n======= INGRESOS ORDENADOS (MAYOR A MENOR) =======\n";
	cout << "Camion: " << camion << ", Chofer: " << chofer << endl;
	cout << fixed << setprecision(2);
	cout << "Dia\t\tIngreso\n";
	for (size_t i = 0; i < ingresos.size(); i++) {
		cout << obtenerNombreDia(i, diasSemana, esSemanaActual) << "\t" << ingresos[i] << "\n";
	}
	cout << "---------------------------------\nTOTAL:\t\t" << total << "\n";

	// 1. Guardar el registro en el archivo CSV (APPEND)
	guardarIngresosCSV_Append(camion, chofer, ingresos, total);
	cout << "Registro guardado en '" << NOMBRE_ARCHIVO_INGRESOS << "'.\n";

	// 2. Mostrar el menu de listas inmediatamente
	limpiarPantalla();
	menuListaIngresosCompleto("METODO BURBUJA");
}

// ---------- MÉTODO SELECCIÓN (Adaptado para List Menu) ----------
void seleccionAsc(vector<double>& costos) {
	for (size_t i = 0; i < costos.size() - 1; i++) {
		size_t minIndex = i;
		for (size_t j = i + 1; j < costos.size(); j++) {
			if (costos[j] < costos[minIndex]) minIndex = j;
		}
		swap(costos[i], costos[minIndex]);
	}
}

void guardarTallerCSV_Append(string camion, string encargado, string taller, const vector<double>& costos, double total) {
	const string nombreArchivo = NOMBRE_ARCHIVO_TALLER;
	bool archivoVacio = false;
	ifstream verificar(nombreArchivo);
	if (verificar.is_open()) {
		verificar.seekg(0, ios::end);
		if (verificar.tellg() == 0) archivoVacio = true;
		verificar.close();
	}
	else {
		archivoVacio = true;
	}

	ofstream archivo(nombreArchivo, ios::app);
	if (archivo.is_open()) {
		if (archivoVacio) {
			archivo << "Camion,Encargado,Taller";
			for (size_t i = 1; i <= costos.size(); ++i) archivo << ",Costo_" << i;
			archivo << ",Total\n";
		}
		archivo << fixed << setprecision(2);
		archivo << camion << "," << encargado << "," << taller;
		for (double val : costos) archivo << "," << val;
		archivo << "," << total << "\n";
		archivo.close();
	}
	else {
		cout << "No se pudo abrir el archivo para guardar los datos.\n";
	}
}
// --------------------------- TALLER (SELECCIÓN) ---------------------------

vector<TallerRecord> cargarTallerCSV() {
	vector<TallerRecord> registros;
	ifstream archivo(NOMBRE_ARCHIVO_TALLER);
	if (!archivo.is_open()) return registros;
	string linea;
	getline(archivo, linea); // Ignorar encabezados

	while (getline(archivo, linea)) {
		stringstream ss(linea);
		string segmento;
		TallerRecord record;

		getline(ss, record.camion, ',');
		getline(ss, record.encargado, ',');
		getline(ss, record.taller, ',');

		record.costos.clear();
		while (getline(ss, segmento, ',')) {
			try {
				if (ss.peek() == EOF || ss.peek() == '\n') { // Último elemento es el total
					record.total = stod(segmento);
					break;
				}
				record.costos.push_back(stod(segmento));
			}
			catch (...) { record.costos.push_back(0.0); }
		}
		registros.push_back(record);
	}
	archivo.close();
	return registros;
}
void guardarTallerCSV_Sobrescribir(const vector<TallerRecord>& registros) {
	ofstream archivo(NOMBRE_ARCHIVO_TALLER, ios::out);
	if (archivo.is_open()) {
		// Encabezados dinámicos para los costos
		archivo << "Camion,Encargado,Taller";
		if (!registros.empty()) {
			for (size_t i = 0; i < registros[0].costos.size(); ++i) {
				archivo << ",Costo_" << (i + 1);
			}
		}
		else {
			archivo << ",Costo_1"; // Poner al menos uno si está vacío
		}
		archivo << ",Total\n";

		archivo << fixed << setprecision(2);
		for (const auto& record : registros) {
			archivo << record.camion << "," << record.encargado << "," << record.taller;
			for (double val : record.costos) archivo << "," << val;
			archivo << "," << record.total << "\n";
		}
		archivo.close();
		cout << "[CSV ACTUALIZADO] La tabla de registros ha sido sobrescrita en '" << NOMBRE_ARCHIVO_TALLER << "'.\n";
	}
	else {
		cout << "!!! ERROR: No se pudo abrir el archivo para sobrescribir los datos.\n";
	}
}

void metodoSeleccion() {
	string camion, encargado, taller;
	cout << "\n======= METODO SELECCION: REGISTRO DE COSTOS DE TALLER (ORDENAMIENTO ASCENDENTE) =======\n";
	cout << "Ingrese el numero de camion: ";
	cin >> camion;
	limpiarBuffer();
	cout << "Ingrese el nombre del encargado: ";
	getline(cin, encargado);
	cout << "Ingrese el nombre del taller: ";
	getline(cin, taller);

	int visitas;
	visitas = pedirEntero("¿Cuantas veces fue al taller este mes?: ");
	if (visitas <= 0) {
		cout << "El numero de visitas debe ser un valor positivo.\n";
		limpiarPantalla();
		return;
	}

	vector<double> costos(visitas);
	cout << "\nIngrese los costos por cada reparacion:\n";
	for (size_t i = 0; i < costos.size(); i++) {
		costos[i] = pedirDouble("Costo de la reparacion #" + to_string(i + 1) + ": ");
	}

	seleccionAsc(costos);
	double total = sumaValores(costos);

	cout << "\n======= COSTOS ORDENADOS (MENOR A MAYOR) =======\n";
	cout << fixed << setprecision(2);
	cout << "Visita\tCosto\n";
	for (size_t i = 0; i < costos.size(); i++) {
		cout << (i + 1) << "\t" << costos[i] << "\n";
	}
	cout << "---------------------------------\nTOTAL:\t" << total << "\n";

	guardarTallerCSV_Append(camion, encargado, taller, costos, total);
	cout << "Registro guardado en '" << NOMBRE_ARCHIVO_TALLER << "'.\n";

	limpiarPantalla();
	menuListaTallerCompleto("METODO SELECCION");
}

// ---------- MÉTODO INSERCIÓN (Sin Menu de Listas aún) ----------
// ---------- MÉTODO INSERCIÓN (Adaptado para List Menu) ----------
void insercionAsc(vector<double>& km) {
	for (size_t i = 1; i < km.size(); i++) {
		double key = km[i];
		int j = static_cast<int>(i) - 1;
		while (j >= 0 && km[j] > key) {
			km[j + 1] = km[j];
			j--;
		}
		km[j + 1] = key;
	}
}

void guardarKmCSV_Append(string camion, string chofer, const vector<double>& km, double total) {
	const string nombreArchivo = NOMBRE_ARCHIVO_KM;
	bool archivoVacio = false;
	ifstream verificar(nombreArchivo);
	if (verificar.is_open()) {
		verificar.seekg(0, ios::end);
		if (verificar.tellg() == 0) archivoVacio = true;
		verificar.close();
	}
	else {
		archivoVacio = true;
	}

	ofstream archivo(nombreArchivo, ios::app);
	if (archivo.is_open()) {
		if (archivoVacio) {
			archivo << "Camion,Chofer";
			for (size_t i = 1; i <= km.size(); ++i) archivo << ",Km_Dia_" << i;
			archivo << ",Total\n";
		}
		archivo << fixed << setprecision(2);
		archivo << camion << "," << chofer;
		for (double val : km) archivo << "," << val;
		archivo << "," << total << "\n";
		archivo.close();
	}
	else {
		cout << "No se pudo abrir el archivo para guardar los datos.\n";
	}
}
}

void metodoInsercion() {
	string camion, chofer;
	cout << "\n======= METODO INSERCION: REGISTRO DE KILOMETRAJE (ORDENAMIENTO ASCENDENTE) =======\n";
	cout << "Ingrese el numero de camion: ";
	cin >> camion;
	limpiarBuffer();
	cout << "Ingrese el nombre del chofer: ";
	getline(cin, chofer);

	int dias;
	dias = pedirEntero("¿Cuantos dias desea registrar kilometraje?: ");
	if (dias <= 0) {
		cout << "El numero de dias debe ser un valor positivo.\n";
		limpiarPantalla();
		return;
	}

	vector<double> km(dias);
	for (size_t i = 0; i < km.size(); i++) {
		km[i] = pedirDouble("Kilometraje del dia #" + to_string(i + 1) + ": ");
	}

	insercionAsc(km);
	double total = sumaValores(km);

	cout << "\n======= KILOMETRAJE ORDENADO (MENOR A MAYOR) =======\n";
	cout << fixed << setprecision(2);
	cout << "Dia\tKm\n";
	for (size_t i = 0; i < km.size(); i++) cout << (i + 1) << "\t" << km[i] << "\n";
	cout << "---------------------------------\nTOTAL:\t" << total << "\n";

	guardarKmCSV_Append(camion, chofer, km, total);
	cout << "Registro guardado en '" << NOMBRE_ARCHIVO_KM << "'.\n";

	limpiarPantalla();
	menuListaKilometrajeCompleto("METODO INSERCION");
}


// ======================= MENÚ PRINCIPAL Y ORDENAMIENTO =======================

void menuOrdenamientoYLista() {
	int opcion = 0;
	while (opcion != 4) {
		// system("cls"); // Descomentar para limpiar
		cout << "\n======== METODOS DE ORDENAMIENTO Y MANIPULACION DE LISTAS ========\n";
		cout << "1. Metodo Burbuja (Ingresos) - C/ Menu de Listas Completo" << endl;
		cout << "2. Metodo Seleccion (Costos de Taller) - C/ Menu de Listas Completo" << endl;
		cout << "3. Metodo Insercion (Kilometraje) - C/ Menu de Listas Completo" << endl;
		cout << "4. Regresar al menu principal" << endl;
		cout << "Seleccione una opcion: ";
		cin >> opcion;
		limpiarBuffer();
		// system("cls"); // Descomentar para limpiar

		switch (opcion) {
		case 1:
			metodoBurbuja();
			break;
		case 2:
			metodoSeleccion();
			break;
		case 3:
			metodoInsercion();
			break;
		case 4:
			cout << "Regresando al menu principal..." << endl;
			break;
		default:
			cout << "Opcion invalida. Intente de nuevo." << endl;
			limpiarPantalla();
			break;
		}
	}
}



// ======================= VENTAS CON PUNTEROS =======================
void menuVentasPunteros() {
	cout << "\n======= MODULO DE VENTAS (USO DE PUNTEROS) =======" << endl;
	cout << "Bienvenido al sistema de venta de boletos." << endl;

	// 1. Declaración de variables locales
	string vendedor;
	int numVenta = 0;
	double totalVenta = 0.0;
	double porcentajeDescuento = 0.0;
	double montoFinal = 0.0;

	// 2. Declaración de PUNTEROS
	string* pVendedor = &vendedor;
	int* pNumVenta = &numVenta;
	double* pTotalVenta = &totalVenta;
	double* pPorcentaje = &porcentajeDescuento;
	double* pFinal = &montoFinal;

	// 3. Solicitar datos usando punteros (Requisito de imagen)
	cout << "Ingrese el nombre del vendedor (Taquillero): ";
	// Usamos getline con el contenido apuntado por pVendedor
	getline(cin, *pVendedor);

	cout << "Ingrese el numero de venta (ID de Boleto): ";
	cin >> *pNumVenta;
	limpiarBuffer();

	cout << "Ingrese el total de la venta (Costo del Boleto): $";
	cin >> *pTotalVenta;
	limpiarBuffer();

	// 4. Definir porcentaje de descuento (Requisito: DEFINIR EL PORCENTAJE)
	cout << "\n--- Seleccione tipo de descuento ---" << endl;
	cout << "1. Estudiante (50%)" << endl;
	cout << "2. INAPAM / Tercera Edad (20%)" << endl;
	cout << "3. General / Sin Descuento (0%)" << endl;
	cout << "4. Otro porcentaje personalizado" << endl;
	cout << "Opcion: ";

	int opcDesc;
	cin >> opcDesc;
	limpiarBuffer();

	switch (opcDesc) {
	case 1: *pPorcentaje = 0.50; break; // 50%
	case 2: *pPorcentaje = 0.20; break; // 20%
	case 3: *pPorcentaje = 0.00; break; // 0%
	case 4:
		cout << "Ingrese el porcentaje manual (ej. 10 para 10%): ";
		cin >> *pPorcentaje;
		*pPorcentaje = *pPorcentaje / 100.0; // Convertir a decimal
		limpiarBuffer();
		break;
	default:
		cout << "Opcion no valida. Se aplicara 0% descuento." << endl;
		*pPorcentaje = 0.0;
	}

	// 5. Calcular total usando punteros
	// Descuento = Total * Porcentaje
	double descuentoAplicado = (*pTotalVenta) * (*pPorcentaje);

	// Total Final = Total - Descuento
	*pFinal = *pTotalVenta - descuentoAplicado;

	// 6. Mostrar resultados
	cout << "\n========== TICKET DE VENTA ==========" << endl;
	cout << "Vendedor: " << *pVendedor << endl;
	cout << "Folio Venta: " << *pNumVenta << endl;
	cout << "Subtotal: $" << fixed << setprecision(2) << *pTotalVenta << endl;
	cout << "Descuento aplicado (" << (*pPorcentaje * 100) << "%): -$" << descuentoAplicado << endl;
	cout << "TOTAL A PAGAR: $" << *pFinal << endl;
	cout << "=====================================" << endl;

	// Pausa para que el usuario pueda leer
	cout << "\nVenta registrada correctamente." << endl;
}


// ======================= MAIN =======================
int main() {
	int opcion = 0;

	// Usamos los datos por defecto creados globalmente
	DatosEmpresa miEmpresa = datosDefault;

	while (opcion != 8) {
		cout << "\n======= MENU PRINCIPAL =======" << endl;
		cout << "1. Datos de la Empresa (Funciones y Estructuras)" << endl;
		cout << "2. Registrar y mostrar chofer (Funciones y Estructuras)" << endl;
		cout << "3. Registrar y mostrar Unidad de Transporte (Funciones y Estructuras)" << endl;
		cout << "4. Ventas (Punteros)" << endl;
		cout << "5. Calculo de sueldo acumulado (Recursividad)" << endl;
		cout << "6. Metodos de busqueda" << endl;
		cout << "7. Registro de Entradas y Salidas de Camiones (Pilas)" << endl;
		cout << "8. Salir" << endl;
		cout << "Seleccione una opcion: ";
		cin >> opcion;
		limpiarBuffer();

		switch (opcion) {
		case 1:
			mostrarEmpresa(miEmpresa);
			pausaYLimpiar();
			break;

		case 2: {
			Chofer nuevoChofer;
			cout << "======= INGRESO DE DATOS DEL CHOFER =======" << endl;
			nuevoChofer.nombre = pedirTexto("Ingrese nombre del chofer: ");
			nuevoChofer.id = pedirEntero("Ingrese numero de chofer: ");
			int anioNacimiento = pedirEntero("Ingrese anio de nacimiento del chofer: ");
			nuevoChofer.edad = 2025 - anioNacimiento;
			nuevoChofer.telefono = pedirTexto("Ingrese telefono del chofer: ");
			nuevoChofer.codigoPostal = pedirEntero("Ingrese codigo postal del chofer: ");
			mostrarChofer(nuevoChofer);
			pausaYLimpiar();
			break;
		}

		case 3: {
			UnidadTransporte nuevaUnidad;
			cout << "======= INGRESO DE DATOS DE LA UNIDAD =======" << endl;
			nuevaUnidad.idCamion = pedirEntero("Ingrese ID del camion: ");
			nuevaUnidad.placas = pedirEntero("Ingrese numero de placas (solo numeros): ");
			nuevaUnidad.color = pedirTexto("Ingrese color del camion: ");
			nuevaUnidad.anio = pedirEntero("Ingrese anio del camion: ");
			nuevaUnidad.choferAsignado = pedirTexto("Ingrese nombre del chofer asignado: ");
			nuevaUnidad.aniosEnEmpresa = pedirEntero("Ingrese anios en la empresa del chofer: ");
			mostrarUnidad(nuevaUnidad);
			pausaYLimpiar();
			break;
		}

		case 4:
			menuVentasPunteros();
			pausaYLimpiar();
			break;

		case 5:
			menuCalculoSueldoRecursivo();
			pausaYLimpiar();
			break;

		case 6:
			menuOrdenamientoYLista();
			break;

		case 7:
			menuCamionesPila();
			break;

		case 8:
			cout << "Saliendo del programa. ¡Hasta pronto!" << endl;
			break;

		default:
			cout << "Opcion invalida. Intente de nuevo." << endl;
			pausaYLimpiar();
			break;
		}
	}

	return 0;
}