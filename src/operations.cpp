/**
 *
 * \file operations.cpp
 *
 * \author
 * \par
 * Javier Carlos Jeronimo Suarez. jcjeronimo@alumnos.upm.es \n
 * \n
 * Asignatura Topologia Digital. Curso 2006-2007\n
 * Departamento de Matematica Aplicada. \n
 * Facultad de Informatica. \n
 * Universidad Politecnica de Madrid. Espana. \n
 * \n
 * Digital Topology Course. 2006-2007 year. \n
 * Applied Mathematics Department. \n
 * Computer Science Faculty. \n
 * Polytechnic University of Madrid. Spain.\n
 *
 * \date 20-12-2006
 *
 * Copyright (C) topologia_digital.exe, 2006. Javier Carlos Jeronimo Suarez.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, OPERATION_MA  02110-1301, USA.
 * 
 */

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#include <fstream>
#include <time.h>
using namespace std;

#include "topologia_digital/operations.h"
#include "topologia_digital/image.h"

using namespace topdig;

/**
 * /////////////////////////////////////////////////////////////////////////////
 * //////////////////////////////////////////////////////////////// constructor
 * /////////////////////////////////////////////////////////////////////////////
 */
operations::operations(string image_filename, string operations_filename, bool debug, t_screen_output number_output_files)
{
	// Crea la imagen desde file
	image *image1;
	image1 = new image (image_filename.c_str(), debug, image::LOAD_BW);

	unsigned short int num_op;
	t_line line_sintax;

	time_t clock_before;
	time_t clock_after;

	t_buffer buffer;
	buffer.pos = buffer.content;

	string output_filename;
	char numero[1024];
	t_operation operation;
	t_argumentos arguments;
	image::t_save_format output_format;

	// Abre el file de operaciones
	fstream operations_file;
	operations_file.open (operations_filename.c_str(), ios_base::in);

	// Bucle de operaciones
	num_op = 1;
	operations_file.seekg (0, ios::beg);

	while (!operations_file.eof())
	{
		// Leemos una linea.
		operations_file.getline (buffer.content, 1024);
		buffer.pos = buffer.content;
		line_sintax = check_line (buffer.content);
		// Comprueba si hemos extraido algo de la linea del file
		if (line_sintax != operations::LINE_OPERATION)
		{
			if (line_sintax == operations::LINE_SINTAX_ERROR)
			{
				cout << "ERROR: Sintax error:" << endl;
				cout << ">>>>>> " << buffer.content << endl;
			}
			continue;
		}

		// Extraemos el tipo de operaciÛn.
		if (!extract_operation (buffer, operation))
		{
			cout << "ERROR: Sintax error (operation not recognized):" << endl;
			cout << ">>>>>> " << buffer.content << endl;
			continue;
		}

		// Extraemos los arguments
		if (!extract_arguments (buffer, operation, arguments))
		{
			cout << "ERROR: Sintax error (argument not recognized):" << endl;
			cout << ">>>>>> " << buffer.content << endl;
			continue;
		}

		// Ejecuta la operaciÛn
		// A cada operation hemos asociado un tipo de formato para save la imagen en archivo,
		// de forma que se vea bien el result. Por ejemplo, en la operation OPERATION_EDGES, el result
		// es un archivo que tiene los puntos del frame_width en negro y los interiores en gris.
		switch (operation)
		{
		case OPERATION_WHITE_FRAME:
			cout << endl << "Applying operation WHITE_FRAME with " << arguments.frame_width << " points frame width..." << endl;
			clock_before = clock();
			image1->frame_white(arguments.frame_width);
			clock_after = clock();
			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_BW;
			break;

		case OPERATION_BLACK_FRAME:
			cout << endl << "Applying operation BLACK_FRAME with " << arguments.frame_width << " points frame width..." << endl;

			clock_before = clock();
			image1->frame_black(arguments.frame_width);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_BW;
			break;

		case OPERATION_BLACK_COMPONENTS:
			cout << endl << "Applying operation BLACK_COMPONENTS with ";
			cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence..." << endl;

			clock_before = clock();
			image1->black_components (arguments.adjacence);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_COMPONENTS;
			break;

		case OPERATION_WHITE_COMPONENTS:
			cout << endl << "Applying operation WHITE_COMPONENTS with ";
			cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence..." << endl;

			clock_before = clock();
			image1->white_components (arguments.adjacence);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_COMPONENTS;
			break;

		case OPERATION_EDGES:
			cout << endl << "Applying operation EDGES with ";
			cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence for the resulting border..." << endl;

			clock_before = clock();
			image1->black_components_edges (arguments.adjacence);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_BORDES;
			break;

		case OPERATION_MAT:
			cout << endl << "Applying operation MAT with ";
			cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence for the distance..." << endl;

			clock_before = clock();
			image1->medial_axis_transformation (arguments.adjacence);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_COMPONENTS;
			break;

		case OPERATION_MA:
			cout << endl << "Applying operation MA with ";
			cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence..." << endl;

			clock_before = clock();
			image1->ma (arguments.adjacence);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_MA;
			break;

		case OPERATION_REBUILD_FROM_MA:
			cout << endl << "Applying operation REBUILD_FROM_MA..." << endl;

			clock_before = clock();
			image1->rebuild_from_ma (arguments.adjacence);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_BW;
			break;

		case OPERATION_SIMPLE_POINTS_FIND:
			cout << endl << "Applying operation SIMPLE_POINTS_FIND_SWEEP with ";
			cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence..." << endl;

			clock_before = clock();
			image1->simple_points (image::SIMPLE_POINTS_FIND_SWEEP, arguments.adjacence);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_SIMPLE_POINTS_FIND;
			break;

		case OPERATION_SIMPLE_POINTS_REMOVE:
			cout << endl << "Applying operation SIMPLE_POINTS_REMOVE with ";
			switch (arguments.sp_algorithm)
			{
			case image::SIMPLE_POINTS_NULL:
				cout << "NULL algorithm (!!!!! this is an error !!!!!)..." << endl;
				break;
			case image::SIMPLE_POINTS_FIND_SWEEP:
				cout << "FIND_SWEEP algorithm (!!!!! this is an error !!!!!)..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_SWEEP:
				cout << "SWEEP algorithm and ";
                cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_BORDERS:
				cout << "BORDERS algorithm and ";
                cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_BORDERS_NO_MA:
				cout << "BORDERS_NO_MA algorithm and ";
                cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_NORTH_FRONTIER:
				cout << "SIMPLE_POINTS_REMOVE_NORTH_FRONTIER algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_EAST_FRONTIER:
				cout << "SIMPLE_POINTS_REMOVE_EAST_FRONTIER algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER:
				cout << "SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_WEST_FRONTIER:
				cout << "SIMPLE_POINTS_REMOVE_WEST_FRONTIER algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_RH_NORTH:
				cout << "SIMPLE_POINTS_REMOVE_RH_NORTH algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_RH_EAST:
				cout << "SIMPLE_POINTS_REMOVE_RH_EAST algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_RH_SOUTH:
				cout << "SIMPLE_POINTS_REMOVE_RH_SOUTH algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_RH_WEST:
				cout << "SIMPLE_POINTS_REMOVE_RH_WEST algorithm..." << endl;
				break;
    		case image::SIMPLE_POINTS_REMOVE_ROS_NORTH:
				cout << "SIMPLE_POINTS_REMOVE_ROS_NORTH algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_ROS_EAST:
				cout << "SIMPLE_POINTS_REMOVE_ROS_EAST algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_ROS_SOUTH:
				cout << "SIMPLE_POINTS_REMOVE_ROS_SOUTH algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_ROS_WEST:
				cout << "SIMPLE_POINTS_REMOVE_ROS_WEST algorithm..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_GH89A1_ODD:
				cout << "SIMPLE_POINTS_REMOVE_GH89A1 algorithm (odd iterations)..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_GH89A1_EVEN:
				cout << "SIMPLE_POINTS_REMOVE_GH89A1 algorithm (even iterations)..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_TSIN_ODD:
				cout << "SIMPLE_POINTS_REMOVE_TSIN algorithm (odd iterations)..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_TSIN_EVEN:
				cout << "SIMPLE_POINTS_REMOVE_TSIN algorithm (even iterations)..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_GH89A2_ODD:
				cout << "SIMPLE_POINTS_REMOVE_GH89A2 algorithm (odd iterations)..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_GH89A2_EVEN:
				cout << "SIMPLE_POINTS_REMOVE_GH89A2 algorithm (even iterations)..." << endl;
				break;
			}

			clock_before = clock();
			image1->simple_points (arguments.sp_algorithm, arguments.adjacence);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_SIMPLE_POINTS_REMOVE;
			break;

		case OPERATION_SEQUENTIAL_SHRINK:
			cout << endl << "Applying operation sequentiall_shrink with ";
			switch (arguments.sp_algorithm)
			{
			case image::SIMPLE_POINTS_NULL:
				cout << "NULL algorithm (!!!!! this is an error !!!!!)..." << endl;
				break;
			case image::SIMPLE_POINTS_FIND_SWEEP:
				cout << "FIND_SWEEP algorithm (!!!!! this is an error !!!!!)..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_SWEEP:
				cout << "SWEEP algorithm and ";
                cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_BORDERS:
				cout << "BORDERS algorithm and ";
                cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_BORDERS_NO_MA:
				cout << "BORDES_NO_MA algorithm and ";
                cout << (arguments.adjacence == image::ADJ_8B_4W? "8N_4B": "4N_8B") << " adjacence..." << endl;
				break;
			case image::SIMPLE_POINTS_REMOVE_NORTH_FRONTIER:
			case image::SIMPLE_POINTS_REMOVE_EAST_FRONTIER:
			case image::SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER:
			case image::SIMPLE_POINTS_REMOVE_WEST_FRONTIER:
			case image::SIMPLE_POINTS_REMOVE_RH_NORTH:
			case image::SIMPLE_POINTS_REMOVE_RH_EAST:
			case image::SIMPLE_POINTS_REMOVE_RH_SOUTH:
			case image::SIMPLE_POINTS_REMOVE_RH_WEST:
    		case image::SIMPLE_POINTS_REMOVE_ROS_NORTH:
			case image::SIMPLE_POINTS_REMOVE_ROS_EAST:
			case image::SIMPLE_POINTS_REMOVE_ROS_SOUTH:
			case image::SIMPLE_POINTS_REMOVE_ROS_WEST:
			case image::SIMPLE_POINTS_REMOVE_GH89A1_ODD:
			case image::SIMPLE_POINTS_REMOVE_GH89A1_EVEN:
			case image::SIMPLE_POINTS_REMOVE_TSIN_ODD:
			case image::SIMPLE_POINTS_REMOVE_TSIN_EVEN:
			case image::SIMPLE_POINTS_REMOVE_GH89A2_ODD:
			case image::SIMPLE_POINTS_REMOVE_GH89A2_EVEN:
				cout << "PARALLEL algorithm (!!!!! this is an error !!!!!)..." << endl;
				break;
			}

			clock_before = clock();
			image1->sequentiall_shrink (arguments.sp_algorithm, arguments.adjacence);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_SIMPLE_POINTS_REMOVE;
			break;

		case OPERATION_PARALLEL_SHRINK:
			cout << endl << "Applying operation parallel_shrink with ";
			switch (arguments.parallel_algorithm)
			{
			case image::PARALLEL_SHRINK_ALGORITHM_NULL:
				cout << "NULL algorithm (!!!!! this is an error !!!!!)..." << endl;
				break;
			case image::PARALLEL_SHRINK_ALGORITHM_FRONTIERS:
				cout << "FRONTIERS algorithm..." << endl;
				break;
			case image::PARALLEL_SHRINK_ALGORITHM_RH:
				cout << "RH algorithm..." << endl;
				break;
			case image::PARALLEL_SHRINK_ALGORITHM_ROS:
				cout << "ROS algorithm..." << endl;
				break;
			case image::PARALLEL_SHRINK_ALGORITHM_GH89A1:
				cout << "GH89A1 algorithm..." << endl;
				break;
			case image::PARALLEL_SHRINK_ALGORITHM_TSIN:
				cout << "TSIN algorithm..." << endl;
				break;
			case image::PARALLEL_SHRINK_ALGORITHM_GH89A2:
				cout << "GH89A2 algorithm..." << endl;
				break;
			}

			clock_before = clock();
			image1->parallel_shrink (arguments.parallel_algorithm, 1);
			clock_after = clock();

			cout << endl << "Spent time = " << (double)(clock_after - clock_before) / CLOCKS_PER_SEC << " seconds." << endl;
			output_format = image::SAVE_SIMPLE_POINTS_REMOVE;
			break;

		default:
			continue;
			break;
		}
		// Si el formato de salida es incremental, generamos
		// un file BMP con los datos actuales de la imagen.
		if (number_output_files == OUTPUT_DIFF)
		{
			sprintf (numero, "_%03d", num_op++);
			// Copiamos el nombre del file
			output_filename = image_filename;
			// Insertamos la numeraciÛn antes de la extensiÛn.
			output_filename.insert (output_filename.length() - 4, numero);
			image1->save (output_filename, output_format);
		}
	}
	if (number_output_files == OUTPUT_RESULT)
	{
		// Genera el BMP con el result final
		output_filename = image_filename;
		output_filename.insert (output_filename.length() - 4, "_FINAL");

		cout << endl << "Storing final result..." << endl;
		image1->save (output_filename, image::SAVE_BW);
	}

	// Cierra el file de operaciones
	operations_file.close();

	// Destruye la imagen
	delete image1;
}

// /////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////// destructor
// /////////////////////////////////////////////////////////////////////////////
operations::~operations(void)
{
	// No hay ninguna estructura que destruir: no hace nada.
}

// /////////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////// check_line
// /////////////////////////////////////////////////////////////////////////////
operations::t_line operations::check_line (char *buffer)
{
	if ((*buffer >= 'a' && *buffer <= 'z') ||
		(*buffer >= 'A' && *buffer <= 'Z'))
		return operations::LINE_OPERATION;

	if ((*buffer == '#'))
		return operations::LINE_COMMENT;

	return operations::LINE_BLANK;
}

// /////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////// extract_operation
// /////////////////////////////////////////////////////////////////////////////
bool operations::extract_operation (t_buffer &buffer,
									t_operation &operation)
{
	// Variables que se usar·n
	enum t_automata
	{
		OPERACION = 1,
		ESPACIO = 2
	};
	t_automata status = OPERACION;
	string text;
	text.clear();

	while (status == OPERACION)
	{
		switch (*buffer.pos)
		{
		case ' ':
			status = ESPACIO;
			buffer.pos++;
			break;
		case '\n':
		case '\0':
			break;
		default:
			text.append (buffer.pos++, 1);
			break;
		}
	}
	if (text.compare("white_frame") == 0)
		operation = OPERATION_WHITE_FRAME;
	else if (text.compare("black_frame") == 0)
		operation = OPERATION_BLACK_FRAME;
	else if (text.compare("black_components") == 0)
		operation = OPERATION_BLACK_COMPONENTS;
	else if (text.compare("white_components") == 0)
		operation = OPERATION_WHITE_COMPONENTS;
	else if (text.compare("edges") == 0)
		operation = OPERATION_EDGES;
	else if (text.compare("medial_axis_transformation") == 0)
		operation = OPERATION_MAT;
	else if (text.compare("ma") == 0)
		operation = OPERATION_MA;
	else if (text.compare("rebuild_from_ma") == 0)
		operation = OPERATION_REBUILD_FROM_MA;
	else if (text.compare("simple_points_find") == 0)
		operation = OPERATION_SIMPLE_POINTS_FIND;
	else if (text.compare("simple_points_remove") == 0)
		operation = OPERATION_SIMPLE_POINTS_REMOVE;
	else if (text.compare("sequential_shrink") == 0)
		operation = OPERATION_SEQUENTIAL_SHRINK;
	else if (text.compare("parallel_shrink") == 0)
		operation = OPERATION_PARALLEL_SHRINK;
	else
		return false;
	return true;
}

// /////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////// extract_arguments
// /////////////////////////////////////////////////////////////////////////////
bool operations::extract_arguments (t_buffer &buffer,
									t_operation operation,
									t_argumentos &arguments)
{
	// Variables que se usar·n
	unsigned short int num_args;
	string text;
	text.clear();
	num_args = 1;
	while (true)
	{
		switch (*buffer.pos)
		{
		case ' ':
		case '\n':
		case '\0':
			switch (operation)
			{
			case OPERATION_WHITE_FRAME:
			case OPERATION_BLACK_FRAME:
				if (num_args == 1)
					arguments.frame_width = (image::dword) atof(text.c_str());
				else
					return false;
				return true;
				break;
			case OPERATION_BLACK_COMPONENTS:
			case OPERATION_WHITE_COMPONENTS:
				if (num_args == 1)
				{
					if (text.compare("8_4") == 0)
						arguments.adjacence = image::ADJ_8B_4W;
					else if (text.compare("4_8") == 0)
						arguments.adjacence = image::ADJ_4B_8W;
					else
						return false;
					return true;
				}
				else
					return false;
				break;
			case OPERATION_EDGES:
				if (num_args == 1)
				{
					if (text.compare("8_4") == 0)
						arguments.adjacence = image::ADJ_8B_4W;
					else if (text.compare("4_8") == 0)
						arguments.adjacence = image::ADJ_4B_8W;
					else
						return false;
					return true;
				}
				else
					return false;
				break;
			case OPERATION_MAT:
				if (num_args == 1)
				{
					if (text.compare("8_4") == 0)
						arguments.adjacence = image::ADJ_8B_4W;
					else if (text.compare("4_8") == 0)
						arguments.adjacence = image::ADJ_4B_8W;
					else
						return false;
					return true;
				}
				else
					return false;
				break;
			case OPERATION_MA:
				if (num_args == 1)
				{
					if (text.compare("8_4") == 0)
						arguments.adjacence = image::ADJ_8B_4W;
					else if (text.compare("4_8") == 0)
						arguments.adjacence = image::ADJ_4B_8W;
					else
						return false;
					return true;
				}
				else
					return false;
				break;
			case OPERATION_REBUILD_FROM_MA:
				if (num_args == 1)
				{
					if (text.compare("8_4") == 0)
						arguments.adjacence = image::ADJ_8B_4W;
					else if (text.compare("4_8") == 0)
						arguments.adjacence = image::ADJ_4B_8W;
					else
						return false;
					return true;
				}
				else
					return false;
				break;
			case OPERATION_SIMPLE_POINTS_FIND:
				return true;
				break;

			case OPERATION_SIMPLE_POINTS_REMOVE:
				if (num_args == 1)
				{
					if (text.compare("sweep") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_SWEEP;
					else if (text.compare("borders") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_BORDERS;
					else if (text.compare("borders_no_ma") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_BORDERS_NO_MA;
					else if (text.compare("north_frontier") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_NORTH_FRONTIER;
					else if (text.compare("east_frontier") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_EAST_FRONTIER;
					else if (text.compare("south_frontier") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER;
					else if (text.compare("west_frontier") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_WEST_FRONTIER;
					else if (text.compare("rh_north") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_RH_NORTH;
					else if (text.compare("rh_east") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_RH_EAST;
					else if (text.compare("rh_south") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_RH_SOUTH;
					else if (text.compare("rh_west") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_RH_WEST;
					else if (text.compare("ros_north") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_ROS_NORTH;
					else if (text.compare("ros_east") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_ROS_EAST;
					else if (text.compare("ros_south") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_ROS_SOUTH;
					else if (text.compare("ros_west") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_ROS_WEST;
					else if (text.compare("gh89a1_odd") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_GH89A1_ODD;
					else if (text.compare("gh89a1_even") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_GH89A1_EVEN;
					else if (text.compare("tsin_odd") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_TSIN_ODD;
					else if (text.compare("tsin_even") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_TSIN_EVEN;
					else if (text.compare("gh89a2_odd") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_GH89A2_ODD;
					else if (text.compare("gh89a2_even") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_GH89A2_EVEN;
					else
						return false;
				}
				else if (num_args == 2)
				{
					
					if (text.compare("8_4") == 0)
						arguments.adjacence = image::ADJ_8B_4W;
					else if (text.compare("4_8") == 0)
						arguments.adjacence = image::ADJ_4B_8W;
					else
						return false;
					return true;
				}
				else
					return false;
				break;
			case OPERATION_SEQUENTIAL_SHRINK:
				if (num_args == 1)
				{
					if (text.compare("sweep") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_SWEEP;
					else if (text.compare("borders") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_BORDERS;
					else if (text.compare("borders_no_ma") == 0)
						arguments.sp_algorithm = image::SIMPLE_POINTS_REMOVE_BORDERS_NO_MA;
					else
						return false;
				}
				else if (num_args == 2)
				{
					
					if (text.compare("8_4") == 0)
						arguments.adjacence = image::ADJ_8B_4W;
					else if (text.compare("4_8") == 0)
						arguments.adjacence = image::ADJ_4B_8W;
					else
						return false;
					return true;
				}
				else
					return false;
				break;
			case OPERATION_PARALLEL_SHRINK:
				if (num_args == 1)
				{
					if (text.compare("frontiers") == 0)
						arguments.parallel_algorithm = image::PARALLEL_SHRINK_ALGORITHM_FRONTIERS;
					else if (text.compare("rh") == 0)
						arguments.parallel_algorithm = image::PARALLEL_SHRINK_ALGORITHM_RH;
					else if (text.compare("ros") == 0)
						arguments.parallel_algorithm = image::PARALLEL_SHRINK_ALGORITHM_ROS;
					else if (text.compare("gh89a1") == 0)
						arguments.parallel_algorithm = image::PARALLEL_SHRINK_ALGORITHM_GH89A1;
					else if (text.compare("tsin") == 0)
						arguments.parallel_algorithm = image::PARALLEL_SHRINK_ALGORITHM_TSIN;
					else if (text.compare("gh89a2") == 0)
						arguments.parallel_algorithm = image::PARALLEL_SHRINK_ALGORITHM_GH89A2;
					else
						return false;
					return true;
				}
				else
					return false;
				break;
			default:
				break;
			}
			text.clear();
			num_args++;
			buffer.pos++;
			break;
		default:
			text.append (buffer.pos++, 1);
			break;
		}
	}
	return false;
}
