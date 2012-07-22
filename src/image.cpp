/**
 *
 * \file image.cpp
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

#include <topologia_digital/image.h>

#include <fstream>
#include <stdio.h>
#include <set>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/barrier.hpp>

using namespace std;

namespace topdig
{

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////////// CONSTRUCTOR
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: cierto
	// POST: Genera una imagen nula con la que no se puede operar.
	//
	image::image ()
	{
		this->im_width = 0;
		this->im_height = 0;
		this->im_status = STATUS_NULL;
		this->im_debug = true;
		this->im_black_components = 0;
		this->im_white_components = 0;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////////// CONSTRUCTOR
	// /////////////////////////////////////////////////////////////////////////////
	image::image (const topdig::image &original_image)
	{
		// Variables
		dword i;
		dword j;
		
		// We copy the atributes of the original image.
		this->im_width = original_image.im_width;
		this->im_height = original_image.im_height;
		this->im_status = original_image.im_status;
		this->im_debug = original_image.im_debug;
		this->im_black_components = original_image.im_black_components;
		this->im_white_components = original_image.im_white_components;
		
		// We copy the image matrix.
		this->im_image_matrix = new t_colour*[this->im_height];
		this->im_pshrink_initial_image_matrix = NULL;
		for (i = 1;
			 i <= this->im_height;
			 i++)
		{
			this->im_image_matrix[(i)-1] = new t_colour[this->im_width];
			for (j = 1;
				 j <= this->im_width;
				 j++)
			{
				// Rellenamos con puntos blancos.
				this->im_image_matrix[(i)-1][(j)-1] = 0;
			}
		}
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////////// CONSTRUCTOR
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: cierto
	// POST: Genera una imagen vacia.
	//
	image::image (
		const dword width,
		const dword heigth,
		const bool debug)
	{
		// Variables que se usaran
		dword i;
		dword j;

		this->im_debug = debug;

		if (this->im_debug)
			cout << endl << "Creating blank image... ";

		this->im_width = width;
		this->im_height = heigth;
		this->im_black_components = 0;
		this->im_white_components = 1;

		if (this->im_debug)
			cout << heigth << "x" << width << "... ";

		// Generamos la matriz de la imagen.
		this->im_image_matrix = new t_colour*[this->im_height];
		this->im_pshrink_initial_image_matrix = NULL;
		for (i = 1;
			 i <= this->im_height;
			 i++)
		{
			this->im_image_matrix[(i)-1] = new t_colour[this->im_width];
			for (j = 1;
				 j <= this->im_width;
				 j++)
			{
				// Rellenamos con puntos blancos.
				this->im_image_matrix[(i)-1][(j)-1] = 0;
			}
		}

		if (this->im_debug)
			cout << " Done." << endl;

		this->im_status = (t_status) ((int) STATUS_EMPTY | (int) STATUS_WHITE_COMPONENTS | (int) STATUS_BLACK_COMPONENTS);
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////////// CONSTRUCTOR
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: cierto
	// POST: Genera una imagen desde los datos contenidos en el archivo indicado.
	//       El archivo debe estar en formato BMP sin compression.
	//       En caso de que se lean correctamente los datos devuelve la imagen
	//       en status STATUS_LOADED. En cualquier otro caso, devuelve la imagen
	//       en status STATUS_NULL (es decir, genera una imagen nula como el
	//       constructor por defecto).
	// 
	image::image (
		const string filename,
        const bool debug,
		const t_load_format color_format)
	{
		// Crea una imagen desde file.
		// Variables que se usarÃ¡n.
		// Para el manejo del file
		fstream file;
		// ComprobaciÃ³n del tamaÃ±o real del file con el inficado en la cabecera
		dword real_size;
		// Para extraer datos del file.
		// Para la cabecera
		word id;
		dword file_size;
		dword reserved;
		dword data_offset;
		dword header_size;
		dword width;
		dword heigth;
		word colour_planes;
		word bpp;
		dword compression;
		dword data_size;
		dword horizontal_resolution;
		dword vertical_resolution;
		// Para la paleta de colores
		byte byte_readed;
		// Para los puntos
		t_colour point_rgb_red;
		t_colour point_rgb_green;
		t_colour point_rgb_blue;
		t_colour colour;
		dword i;
		dword j;
		word offset;

		this->im_debug = debug;
		if (this->im_debug)
			cout << "Loading image from bitmap file \"" << filename.c_str() << "\"" << "... ";

		// ///////////////////////////////////
		// Abrimos el file
		file.open (filename.c_str());

		// Extraemos el tamaÃ±o real del file
		file.seekg (0, ios::end);
		real_size = file.tellg();
		file.seekg (0, ios::beg);

		// ///////////////////////////////////
		// Leemos la cabecera
		id = read_word (file);
		file_size = read_dword (file);
		reserved = read_dword (file);
		data_offset = read_dword (file);
		header_size = read_dword (file);
		width = read_dword (file);
		heigth = read_dword (file);
		colour_planes = read_word (file);
		bpp = read_word (file);
		compression = read_dword (file);
		data_size = read_dword (file);
		horizontal_resolution = read_dword (file);
		vertical_resolution = read_dword (file);

		if (im_debug)
		{
			cout << "(" << width << "x";
			cout << heigth << "-";
			cout << bpp << "bpp)... ";
		}

		// ///////////////////////////////////
		// Comprobamos que la cabecera es correcta

		// Estos campos deben ser correctos.
		if (id != 0x4d42 || compression != 0 || bpp < 24)
		{
			// Mostramos el error.
			if (id != 0x4d42)
                cerr << "[ERROR]::image::Incorrect file format." << endl;

			if (compression != BI_RGB)
				cerr << "[ERROR]::image::image()::Compression type not supported." << endl;

			if (bpp < 24)
				cerr << "[ERROR]::image::image()::bpp not supported." << endl;

			// Generamos la imagen nula.
			this->im_width = 0;
			this->im_height = 0;
			this->im_black_components = 0;
			this->im_white_components = 0;
			this->im_status = STATUS_NULL;

			return;
		}

		// Permitimos los siguientes errores para evitar cabeceras incorrectas.
		if (real_size != file_size)
            cerr << "[WARNING]::image::image()::Real file size doesn't match head's file size." << endl;

		if (data_offset + data_size != file_size)
			cerr << "[WARNING]::image::image()::Data size doesn't match head's data size." << endl;

		// ///////////////////////////////////
		// Copiamos los datos de la cabecera a la imagen
		this->im_width = width;
		this->im_height = heigth;

		// ///////////////////////////////////
		// Reservamos el espacio para la matriz que alojara la imagen.
		this->im_pshrink_initial_image_matrix = NULL;
		this->im_image_matrix = new t_colour*[this->im_height];
		for (i = 1;
			 i <= this->im_height;
			 i++)
		{
			this->im_image_matrix[(i)-1] = new t_colour[this->im_width];
		}

		if (this->im_debug)
			cout << "reading image points... ";
		// ///////////////////////////////////
		// Leemos los puntos de la imagen.
		// Nos desplazamos hasta el point donde comienzan los datos: 0 + data_offset
		file.seekg (data_offset, ios_base::beg);

		// Cargamos los puntos de izquierda a derecha
		switch (bpp)
		{
		case 24: // bpp = 24 (8bits * 3 colores) = (1 byte * 3 colores)
			// En cada point encontramos los 3 bytes (B,G,R)
			// Calculamos el relleno en bytes de cada fila, ya que el numero de bytes
			// almacenado por cada fila debe ser multiplo de 4 (en el file).
			offset = ((this->im_width*3) % 4);
			if (offset > 0)
				offset = 4 - offset;

			for (i = heigth; i >= 1; i--)
			{
				for (j = 1;
					 j <= this->im_width;
					 j++)
				{
					// Leemos el primer byte: azul
					byte_readed = read_byte (file);
					point_rgb_blue = byte_readed;

					// Leemos el segundo byte: verde
					byte_readed = read_byte (file);
					point_rgb_green = byte_readed;

					// Leemos el tercer byte: rojo
					byte_readed = read_byte (file);
					point_rgb_red = byte_readed;

					// Transformamos el colour del point en 0 (sin colour) o 1 (con colour).
					switch (color_format)
					{
					case LOAD_BW:
						colour = 1-(point_rgb_red/255);
						break;
					case LOAD_RED:
						colour = point_rgb_red;
						if (colour > 128)
							colour = 0;
						else
							colour = 1;
						break;
					case LOAD_GREEN:
						colour = point_rgb_green;
						if (colour > 128)
							colour = 0;
						else
							colour = 1;
						break;
					case LOAD_BLUE:
						colour = point_rgb_blue;
						if (colour > 128)
							colour = 0;
						else
							colour = 1;
						break;
					case LOAD_AVERAGE:
						colour = (point_rgb_red + point_rgb_green + point_rgb_blue)/3;
						if (colour > 128)
							colour = 0;
						else
							colour = 1;
						break;
					}
					// Actualizamos el valor en la matriz.
					this->im_image_matrix[(i)-1][(j)-1] = colour;
				} // for (columnas)

				// Saltamos el posible relleno
				file.seekg (offset, ios_base::cur);
			} // for (filas)

			break;
		case 32: // bpp = 32 (8bits * 4 colores) = (1 byte * 4 colores)
			// En cada point encontramos los 4 bytes (B,G,R,U)
			offset = ((width*4) % 4);
			if (offset > 0)
				offset = 4 - offset;

			for (i = this->im_height;
				 i >= 1;
				 i--)
			{
				for (j = 1;
					 j <= this->im_width;
					 j++)
				{
					// Leemos el primer byte: azul
					byte_readed = read_byte (file);
					point_rgb_blue = byte_readed;

					// Leemos el segundo byte: verde
					byte_readed = read_byte (file);
					point_rgb_green = byte_readed;

					// Leemos el tercer byte: rojo
					byte_readed = read_byte (file);
					point_rgb_red = byte_readed;

					// Saltamos el valor sin uso
					byte_readed = read_byte (file);

					// Transformamos el colour del point en 0 (sin colour) o 1 (con colour).
					switch (color_format)
					{
					case LOAD_BW:
						colour = 1-(point_rgb_red/255);
						break;
					case LOAD_RED:
						colour = point_rgb_red;
						if (colour > 128)
							colour = 0;
						else
							colour = 1;
						break;
					case LOAD_GREEN:
						colour = point_rgb_green;
						if (colour > 128)
							colour = 0;
						else
							colour = 1;
						break;
					case LOAD_BLUE:
						colour = point_rgb_blue;
						if (colour > 128)
							colour = 0;
						else
							colour = 1;
						break;
					case LOAD_AVERAGE:
						colour = (point_rgb_red + point_rgb_green + point_rgb_blue)/3;
						if (colour > 128)
							colour = 0;
						else
							colour = 1;
						break;
					}
					// Actualizamos el valor en la matriz.
					this->im_image_matrix[(i)-1][(j)-1] = colour;
				} // for (columnas)

				// Saltamos el posible relleno de la fila
				file.seekg (offset, ios_base::cur);
			} // for (filas)

			break;
		} // switch

		// ///////////////////////////////////
		// Cerramos el file
		file.close();

		// Cambiamos el atributo que indica el status de la imagen.
		this->im_status = STATUS_LOADED;

		if (this->im_debug)
			cout << " Done." << endl;

	}

	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////// DESTRUCTOR
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: cierto
	// POST: Borra los datos de memoria dinamica que no se eliminaran de forma
	//       automatica al destruir el objeto.
	// 
	image::~image()
	{
		// Variables que se usaran
		dword i;

		// Liberamos la memoria de la matriz.
		for (i = 1;
			 i <= this->im_height;
			 i++)
		{
			delete this->im_image_matrix[(i)-1];
		}
	}
	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////////////////////////////// print
	// /////////////////////////////////////////////////////////////////////////////
	void image::print (
		const t_screen_output output_format)
	{
		print (this->im_image_matrix, output_format);
	}
	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////////////////////////////// print
	// /////////////////////////////////////////////////////////////////////////////
	void image::print (
		t_colour ** matrix,
		const t_screen_output output_format)
	{
		// Variables que se usaran.
		dword i;
		dword j;

		if ((this->im_status & STATUS_NULL) != 0)
		{
			cout << "NULL" << endl;
		}
		else if ((this->im_status & (STATUS_EMPTY | STATUS_LOADED)) != 0)
		{
			// Recorre la matrz y la muestra por pantalla
			switch (output_format)
			{
			case SCREEN_OUTPUT_ASCII:
				// Muestra la imagen con cuadraditos (unicode).
				printf ("#");
				for (j = 1;
					 j <= this->im_width;
					 j++)
					printf ("#");
				{
				printf ("#\n");
				}
				for (i = 1;
					 i <= this->im_height;
					 i+=2)
				{
					printf ("#");
					for (j = 1;
						 j <= this->im_width;
						 j++)
					{
						// Como debemos comprimir dos filas en una sola a la hora de mostrarla
						// (ya que usaremos los simbolos {0xDF='▀',0xDC='▄',0xDB='█',' '}) debemos comprobar si
						// el numero de filas es par o impar.
						if (this->im_height % 2 == 0)
						{
							// Numero de filas pares
							if (matrix[(i)-1][(j)-1] != 0)
							{
								if (matrix[(i+1)-1][(j)-1] != 0)
									printf ("%c", 0xDB); // 0xDB='█'
								else
									printf ("%c", 0xDF); // 0xDF='▀'
							}
							else
							{
								if (matrix[(i+1)-1][(j)-1] != 0)
									printf ("%c", 0xDC); // 0xDC='▄'
								else
									printf (" ");
							}
						}
						else
						{
							// Numero de filas impares
							if (matrix[(i)-1][(j)-1] != 0)
							{
								if (i < this->im_height && matrix[(i+1)-1][(j)-1] != 0)
									printf ("%c", 0xDB); // 0xDB='█'
								else
									printf ("%c", 0xDF); // 0xDF='▀'
							}
							else
							{
								if (i < this->im_height && matrix[(i+1)-1][(j)-1] != 0)
									printf ("%c", 0xDC); // 0xDC='▄'
								else
									printf (" ");
							}
						}
					}
					printf ("#\n");
				}
				printf ("#");
				for (j = 1;
					 j <= this->im_width;
					 j++)
				{
					printf ("#");
				}
				printf ("#\n");
				break;
			case SCREEN_OUTPUT_COMPONENTS:
				// Muestra la imagen usando la numeracion de las components,
				// queda feo si hay mas de 99 components negras (01,02,..,99)
				// o mas de 9 components blancas (-1,-2,..,-9)
				printf ("##");
				for (j = 1;
					 j <= this->im_width;
					 j++)
				{
					printf ("##");
				}
				printf ("##\n");
				for (i = 1;
					 i <= this->im_height;
					 i++)
				{
					printf ("##");
					for (j = 1;
						 j <= this->im_width;
						 j++)
					{
						printf ("%02d", matrix[(i)-1][(j)-1]);
					}
					printf ("##\n");
				}
				printf ("##");
				for (j = 1;
					 j <= this->im_width;
					 j++)
				{
					printf ("##");
				}
				printf ("##\n");
				break;
			}
		}
		return;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////// status
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: cierto
	//
	// POST: Devuelve el status actual de la imagen.
	//
	// POST: result = imagen.im_status
	//
	image::t_status image::status (void)
	{
		return this->im_status;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////////////////////////////// borde
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: cierto
	//
	// POST: Anade un borde a la imagen del colour y width indicados, cambiando sus
	//       dimensiones a n+borde*2, m+borde*2.
	//
	// POST: result = imagen_nueva
	//
	//       DONDE:                      |  colour                       si (i=1 O i=imagen.heigth +2*width) Y
	//                                   |                                  (j=1 O j=imagen.width +2*width)
	//              imagen_nueva[i][j] = |
	//                                   |  imagen[i-width][j-width]     si (1 < i < heigth -2*width) Y
	//                                   |                                  (1 < i < width -2*width)
	//
	//              i = 1..imagen.heigth + 2*width
	//              j = 1..imagen.width + 2*width
	//
	bool image::frame_colour (
		const t_colour colour,
		const dword frame_width)
	{
		// Variables que se usaran.
		dword i;
		dword j;
		dword current_height;

		// Nueva matriz de la imagen.
		t_colour **new_matrix;
		t_colour **original_matrix;

		// Reservamos memoria para la nueva matriz y la vamos rellenando.
		// Si el bloque falla es por un error en la reserva de memoria.
		try
		{
			new_matrix = new t_colour*[this->im_height + 2 * frame_width];
			if (new_matrix == 0)
				throw ERROR_MEMORY_PROBLEM;

			for (j = 1;
				 j <= im_height + 2 * frame_width;
				 j++)
			{
				new_matrix[j-1] = new t_colour[this->im_width + 2 * frame_width];
				if (new_matrix[j-1] == 0)
					throw ERROR_MEMORY_PROBLEM;

				for (i = 1;
					 i <= this->im_width + 2 * frame_width;
					 i++)
				{
					// Copiamos la matriz antigua, excepto el borde, que lo rellenamos con
					// el valor dado.
					if (j <= frame_width ||
						j >= this->im_height + frame_width + 1 ||
						i <= frame_width ||
						i >= this->im_width + frame_width + 1)
					{
						new_matrix[j-1][i-1] = colour;
					}
					else
						new_matrix[j-1][i-1] = this->im_image_matrix [j-frame_width-1][i-frame_width-1];
				}
			}
		}
		catch (t_error error)
		{
			if (error == ERROR_MEMORY_PROBLEM)
			{
                cerr << "[ERROR]::image::frame_colour()::Insufficient memory." << endl;
				return false;
			}
			else
			{
				cerr << "[ERROR]::image::frame_colour()::Big code error?." << endl;
				return false;
			}
		}
		// Cambiamos la matriz y las dimensiones en el objeto.
		original_matrix = this->im_image_matrix;
		this->im_image_matrix = new_matrix;
		current_height = this->im_height;

		this->im_width += frame_width * 2;
		this->im_height += frame_width * 2;

		// Liberamos la memoria de la matriz antigua.
		for (j = 1;
			 j <= current_height;
			 j++)
		{
			delete original_matrix[j-1];
		}

		// Si no ha habido error devolvemos verdadero.
		return true;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////////// frame_black
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: cierto
	//
	// POST: Anade un borde negro del width indicado a la imagen, cambiando sus
	//       dimensiones a n+2*borde, m+2*borde.
	//
	// POST: result = borde (1, borde)
	//
	bool image::frame_black (
		const dword frame_width)
	{
		if ((this->im_status & STATUS_NULL) != 0)
		{
			cerr << "[ERROR]::image::frame_black()::NULL image found." << endl;
			return false;
		}
		if (frame_colour (1, frame_width))
		{
			this->im_status = (t_status) ((int) this->im_status | (int) STATUS_BLACK_FRAME);
			return true;
		}
		return false;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////////////// frame_white
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: cierto
	//
	// POST: Anade un borde negro del width indicado a la imagen, cambiando sus
	//       dimensiones a n+2*borde, m+2*borde.
	//
	// POST: result = borde (0, width)
	//
	bool image::frame_white (
		const dword frame_width)
	{
		if ((this->im_status & STATUS_NULL) != 0)
		{
			cerr << "[ERROR]::image::frame_white()::NULL image found." << endl;
			return false;
		}
		if (frame_colour (0, frame_width))
		{
			this->im_status = (t_status) ((int) this->im_status | (int) STATUS_WHITE_FRAME);
			return true;
		}
		return false;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////////// black_components
	// /////////////////////////////////////////////////////////////////////////////
	//
	bool image::black_components (
		const t_adjacence adjacence)
	{
		if ((this->im_status & STATUS_NULL) != 0)
		{
			cerr << "[ERROR]::image::black_components()::NULL image found." << endl;
			return false;
		}
		// Variables que se usaran
		t_colour result;

		// Busca components
		result = components (adjacence);
		if (result < 0)
			return false;

		// Establece el status de la imagen
		this->im_status = (t_status) ((int) this->im_status | (int) STATUS_BLACK_COMPONENTS);
		this->im_black_components = (dword) result;

		return true;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////////// white_components
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: Cierto
	//
	// POST: Extrae las components de los puntos blancos, etiquetandolas con numeros
	//       negativos.
	//
	//                               NO FUNCIONA
	//
	bool image::white_components (
		const t_adjacence adjacence)
	{
		if ((this->im_status & STATUS_NULL) != 0)
		{
			cerr << "[ERROR]::image::white_components()::NULL image found." << endl;
			return false;
		}
		// Variables que se usaran
		dword i;
		dword j;
		t_colour result;

		// Invierte la matriz: transforma los negativos en positivos, los 
		// positivos en negativos y los puntos blancos (0) en 1 (negros):
		//
		//    0 ->  1
		//    x -> -x
		//   -x ->  x
		for (i = 1;
			 i <= this->im_height;
			 i++)
		{
			for (j = 1;
				 j <= this->im_width;
				 j++)
			{
				if (this->im_image_matrix[(i)-1][(j)-1] == 0)
				{
					this->im_image_matrix[(i)-1][(j)-1] = 1;
				}
				else
				{
					this->im_image_matrix[(i)-1][(j)-1] = - this->im_image_matrix[(i)-1][(j)-1];
				}
			}
		}

		// Busca components negras (medial_axis_transformation(i,j)>0)
		if (adjacence == ADJ_8B_4W)
		{
			result = components (ADJ_4B_8W);
		}
		else
		{
			result = components (ADJ_8B_4W);
		}

		// Invierte la matriz: transforma los negativos en positivos y los 
		// positivos en negativos (ahora ya no puede haber puntos blancos (0):
		//
		//    x -> -x
		//   -x ->  x
		for (i = 1;
			 i <= this->im_height;
			 i++)
		{
			for (j = 1;
				 j <= this->im_width;
				 j++)
			{
				if (this->im_image_matrix[(i)-1][(j)-1] == 0)
				{
					this->im_image_matrix[(i)-1][(j)-1] = 1;
				}
				else
				{
					this->im_image_matrix[(i)-1][(j)-1] = - this->im_image_matrix[(i)-1][(j)-1];
				}
			}
		}

		if (result < 0)
			return false;

		// Establece el status de la imagen
		this->im_status = (t_status) ((int) this->im_status | (int) STATUS_WHITE_COMPONENTS);
		this->im_white_components = (dword) result;

		return true;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////// components
	// /////////////////////////////////////////////////////////////////////////////
	//
	image::t_colour image::components (
		const t_adjacence adjacence)
	{
		if ((im_status & STATUS_WHITE_FRAME) == 0)
		{
			cerr << "[ERROR]::image::components()::not STATUS_WHITE_FRAME." << endl;
			return false;
		}

		// Variables que se usaran.
		dword i;
		dword j;
		dword k;
		t_colour component;
		dword adjacents[4];
		dword minor;
		bool is_first;
		bool do_search;
		t_colour minor_found;
		int index;
		t_equiv equivalent_components;
		t_colour current_colour;
		// Para aplicar las components optimizadas a la matriz.
		bool found;
		// Para recorrer la lista de equivalencias de components
		t_components_set::iterator iterator1;
		t_components_set::iterator iterator2;
		t_components_set::iterator temp_iterator;

		if (im_debug)
			cout << "   Looking for components... ";

		if (im_debug)
			cout << "step 1... ";

		// Realizamos una primera pasada: izquierda -> derecha Y arriba -> abajo
		// En esta pasada se generan components nuevas.
		component = 1;
		index = 0;
		for (i = 2;
			 i <= this->im_height - 1;
			 i++)
		{
			for (j = 2;
				 j <= im_width - 1;
				 j++)
			{
				if (this->im_image_matrix[(i)-1][(j)-1] > 0)
				{
					// Es un point negro y estamos buscando components en negros.
					// Nueva component
					if (this->im_image_matrix[(i)-1][(j-1)-1] <= 0 &&
						this->im_image_matrix[(i-1)-1][(j)-1] <= 0 &&
						(adjacence == ADJ_4B_8W ||
						(this->im_image_matrix[(i-1)-1][(j-1)-1] <= 0 &&
						 this->im_image_matrix[(i-1)-1][(j+1)-1] <= 0)
						)
						)
					{
						this->im_image_matrix[(i)-1][(j)-1] = component++;
						equivalent_components.c1 = this->im_image_matrix[(i)-1][(j)-1];
						equivalent_components.c2 = this->im_image_matrix[(i)-1][(j)-1];
						if (
							this->im_equivalent_components.empty() ||
							this->im_equivalent_components.find (equivalent_components) == this->im_equivalent_components.end()
							)
						{
							this->im_equivalent_components.insert (equivalent_components);
						}
					}
					// Igual que el de la izquierda.
					else if (this->im_image_matrix[(i)-1][(j-1)-1] > 0 &&
							 this->im_image_matrix[(i-1)-1][(j)-1] <= 0 &&
								(adjacence == ADJ_4B_8W ||
									(this->im_image_matrix[(i-1)-1][(j-1)-1] <= 0 &&
									 this->im_image_matrix[(i-1)-1][(j+1)-1] <= 0)
								)
							)
					{
						this->im_image_matrix[(i)-1][(j)-1] = this->im_image_matrix[(i)-1][(j-1)-1];
					}
					// Igual que el de arriba.
					else if (this->im_image_matrix[(i)-1][(j-1)-1] <= 0 &&
							 this->im_image_matrix[(i-1)-1][(j)-1] > 0 &&
								(adjacence == ADJ_4B_8W ||
									(this->im_image_matrix[(i-1)-1][(j-1)-1] <= 0 &&
									 this->im_image_matrix[(i-1)-1][(j+1)-1] <= 0)
								)
							)
					{
						this->im_image_matrix[(i)-1][(j)-1] = this->im_image_matrix[(i-1)-1][(j)-1];
					}
					// Igual que el de arriba a la izquierda (solo si 8N_4B).
					else if (adjacence == ADJ_8B_4W &&
							 this->im_image_matrix[(i)-1][(j-1)-1] <= 0 &&
							 this->im_image_matrix[(i-1)-1][(j)-1] <= 0 &&
							 this->im_image_matrix[(i-1)-1][(j-1)-1] > 0 &&
							 this->im_image_matrix[(i-1)-1][(j+1)-1] <= 0)
					{
						this->im_image_matrix[(i)-1][(j)-1] = this->im_image_matrix[(i-1)-1][(j-1)-1];
					}
					// Igual que el de arriba a la derecha (solo si 8N_4B).
					else if (adjacence == ADJ_8B_4W &&
							 this->im_image_matrix[(i)-1][(j-1)-1] <= 0 &&
							 this->im_image_matrix[(i-1)-1][(j)-1] <= 0 &&
							 this->im_image_matrix[(i-1)-1][(j-1)-1] <= 0 &&
							 this->im_image_matrix[(i-1)-1][(j+1)-1] > 0)
					{
						this->im_image_matrix[(i)-1][(j)-1] = this->im_image_matrix[(i-1)-1][(j+1)-1];
					}
					// Igual que el de arriba, izquierda y diagonal arriba-izquierda (los tres son iguales).
					else if (this->im_image_matrix[(i)-1][(j-1)-1] > 0 &&
							 this->im_image_matrix[(i)-1][(j-1)-1] == this->im_image_matrix[(i-1)-1][(j)-1] &&
								(adjacence == ADJ_4B_8W ||
									(this->im_image_matrix[(i-1)-1][(j-1)-1] > 0 &&
									 this->im_image_matrix[(i-1)-1][(j-1)-1] == this->im_image_matrix[(i-1)-1][(j+1)-1])
								)
							)
					{
						this->im_image_matrix[(i)-1][(j)-1] = this->im_image_matrix[(i)-1][(j-1)-1];
					}
					// Igual que el de arriba o izquierda o diagonal arriba-izquierda (es igual a alguno de ellos).
					else
					{
						// Igual que la minor de las components adjacents que sean distintas de cero.
						//
						//     2 0 3
						//     1 x
						//
						// Busca la minor de las components
						adjacents [0] = this->im_image_matrix[(i-1)-1][(j)-1];
						adjacents [1] = this->im_image_matrix[(i)-1][(j-1)-1];
						if (adjacence == ADJ_8B_4W)
						{
							adjacents [2] = this->im_image_matrix[(i-1)-1][(j-1)-1];
							adjacents [3] = this->im_image_matrix[(i-1)-1][(j+1)-1];
						}
						is_first = true;
						for (k = 0;
							 k < 4;
							 k++)
						{
							if (adjacents[k] > 0 &&
									(is_first || adjacents[k] < adjacents[minor])
								)
							{
								if (k < 2 || adjacence == ADJ_8B_4W)
								{
									minor = k;
									is_first = false;
								}
							}
						}
						// Copiamos la component minor al point
						this->im_image_matrix[(i)-1][(j)-1] = adjacents[minor];
						// Creamos correspondiencias entre las demas components y la minor
						for (k = 0;
							 k < 4;
							 k++)
						{
							if (adjacents[k] > 0 && k != minor)
							{
								if (k < 2 || adjacence == ADJ_8B_4W)
								{
									equivalent_components.c1 = adjacents[minor];
									equivalent_components.c2 = adjacents[k];
									if (
										this->im_equivalent_components.empty() ||
										this->im_equivalent_components.find (equivalent_components) == this->im_equivalent_components.end()
										)
									{
										this->im_equivalent_components.insert (equivalent_components);
									}
								}
							}
						}
					} // if (distintas combinaciones)
				} // if (point negro)
			} // for (width)
		} // for (heigth)

		if (im_debug)
			cout << "optimizaing components... ";

		// Minimiza la lista de equivalencias: (1,3)...(3,4) ===> (1,3)...(1,4)
		for (iterator1 = this->im_equivalent_components.begin();
			 iterator1 != this->im_equivalent_components.end();
			 iterator1++)
		{
			for (iterator2 = this->im_equivalent_components.begin();
				 iterator2 != iterator1;
				 iterator2++)
			{
				//  it2       it1         it2       it1
				// (x,y) ... (a,a) ====> ignoramos        y <> a
				if ((*iterator1).c1 == (*iterator1).c2 &&
					(*iterator2).c2 != (*iterator1).c1)
				{
				}
				//  it2       it1         it2       it1
				// (a,x) ... (x,b) ====> (a,x) ... (a,b)
				else if ((*iterator1).c1 == (*iterator2).c2 &&
						 (*iterator2).c2 != (*iterator2).c1)
				{
					equivalent_components.c1 = (*iterator2).c1;
					equivalent_components.c2 = (*iterator1).c2;
					this->im_equivalent_components.erase (iterator1);
					this->im_equivalent_components.insert (equivalent_components);
					iterator1 = this->im_equivalent_components.find (equivalent_components);
					iterator2 = this->im_equivalent_components.begin();
					break;

				}
				//  it2       it1         it2       it1
				// (a,x) ... (b,x) ====> (a,x) ... (a,b)	a < b
				else if ((*iterator1).c2 == (*iterator2).c2 &&
						 (*iterator1).c1 > (*iterator2).c1)
				{
					equivalent_components.c1 = (*iterator2).c1;
					equivalent_components.c2 = (*iterator1).c1;
					this->im_equivalent_components.erase (iterator1);
					this->im_equivalent_components.insert (equivalent_components);
					iterator1 = this->im_equivalent_components.find (equivalent_components);
					iterator2 = this->im_equivalent_components.begin();
					break;
				}
			}
		}

		if (this->im_debug)
			cout << "minimizing components labels... ";

		// Usa el minor numero de components (siendo la numeracion de cada una de ellas, la minor posible).
		component = 1;
		i = 1;
		do_search = true;
		while (do_search && i < this->im_equivalent_components.size())
		{
			found = false;
			for (iterator1 = this->im_equivalent_components.begin();
				 iterator1 != this->im_equivalent_components.end();
				 iterator1++)
			{
				if ((*iterator1).c1 >= component && (!found || (*iterator1).c1 < minor_found))
				{
					found = true;
					minor_found = (*iterator1).c1;
				}
			}
			if (found && minor_found != component)
			{
				// La minor numeracion de component found se puede disminuir
				for (iterator1 = this->im_equivalent_components.begin();
					 iterator1 != this->im_equivalent_components.end();
					 iterator1++)
				{
					if ((*iterator1).c1 == minor_found)
					{
						// Podemos optimizar esta posicion
						equivalent_components.c1 = component;
						equivalent_components.c2 = (*iterator1).c2;
						current_colour = (*iterator1).c1;
						// Hacemos la sustitucion.
						this->im_equivalent_components.erase (iterator1);
						this->im_equivalent_components.insert (equivalent_components);
						// Pero hay que modificar los puntos que ya tengan como numero de component la que estamos
						// simplificando, con lo que hay que anadir otra regla de simplificacion
						equivalent_components.c1 = component;
						equivalent_components.c2 = current_colour;
						this->im_equivalent_components.insert (equivalent_components);
						iterator1 = this->im_equivalent_components.begin();
					}
				}
				component++;
			}
			else if (minor_found == component)
				component++;
			else
				do_search = false;
			i++;
		}

		if (im_debug)
			cout << "applying components labels... ";

		// Recorremos la matriz
		for (i = 1;
			 i < this->im_height;
			 i++)
		{
			for (j = 1;
				 j < this->im_width;
				 j++)
			{
				// Buscamos una corresponencia entre el numero de component y uno equivalente (optimizado).
				do_search = true;
				iterator1 = this->im_equivalent_components.begin();
				while (do_search && iterator1 != this->im_equivalent_components.end())
				{
					// Si hay equivalent_components, sustituimos el numero de la component.
					if ((*iterator1).c2 == this->im_image_matrix[(i)-1][(j)-1])
					{
						this->im_image_matrix[(i)-1][(j)-1] = (*iterator1).c1;
						do_search = false;
					}
					else if ((*iterator1).c2 > this->im_image_matrix[(i)-1][(j)-1])
						do_search = false;
					else
						iterator1++;
				}
			}
		}

		// Actualizamos el status de la imagen
		this->im_status = (t_status) ((int) this->im_status | (int) STATUS_BLACK_COMPONENTS);

		if (this->im_debug)
			cout << " Done." << endl;

		// Devuelve la llamada indicando el numero de components que ha encontrado.
		return component;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////// black_components_edges
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: Cierto
	//
	// POST: Devuelve la matriz con los black_components_edges etiquetados con '2' y los puntos
	//       del interior etiquetados con '1'.
	//
	bool image::black_components_edges (
		const t_adjacence adjacence)
	{
		if ((this->im_status & STATUS_NULL) != 0)
		{
			cerr << "[ERROR]::image::black_components_edges()::NULL image." << endl;
			return false;
		}
		// Variables que se usaran.
		dword i;
		dword j;
		t_adjacence adj;

		if (this->im_debug)
			cout << "   Looking for black components borders... ";

		// Nos dan la adjacence del borde resultante, con lo que la adjacence
		// que debemos usar al buscar black_components_edges es la contraria.
		if (adjacence == ADJ_8B_4W)
			adj = ADJ_4B_8W;
		else
			adj = ADJ_8B_4W;

		for (i = 2;
			 i <= this->im_height-1;
			 i++)
		{
			for (j = 2;
				 j <= this->im_width - 1;
				 j++)
			{
				if (this->im_image_matrix[(i)-1][(j)-1] > 0)
				{
					// Comprobamos si el point es borde
					if (this->im_image_matrix[(i-1)-1][(j)-1] <= 0 ||
						this->im_image_matrix[(i)-1][(j-1)-1] <= 0 ||
						this->im_image_matrix[(i)-1][(j+1)-1] <= 0 ||
						this->im_image_matrix[(i+1)-1][(j)-1] <= 0 ||
							(adj == ADJ_8B_4W &&
								(this->im_image_matrix[(i-1)-1][(j-1)-1] <= 0 ||
								 this->im_image_matrix[(i-1)-1][(j+1)-1] <= 0 ||
								 this->im_image_matrix[(i+1)-1][(j-1)-1] <= 0 ||
								 this->im_image_matrix[(i+1)-1][(j+1)-1] <= 0)
							)
						)
					{
						// Es un point del borde
						this->im_image_matrix[(i)-1][(j)-1] = 2;
					}
					else if (this->im_image_matrix[(i-1)-1][(j)-1] > 0 ||
							 this->im_image_matrix[(i)-1][(j-1)-1] > 0 ||
							 this->im_image_matrix[(i)-1][(j+1)-1] > 0 ||
							 this->im_image_matrix[(i+1)-1][(j)-1] > 0 ||
							 (adj == ADJ_8B_4W &&
								(this->im_image_matrix[(i-1)-1][(j-1)-1] > 0 ||
								 this->im_image_matrix[(i-1)-1][(j+1)-1] > 0 ||
								 this->im_image_matrix[(i+1)-1][(j-1)-1] > 0 ||
								 this->im_image_matrix[(i+1)-1][(j+1)-1] > 0)
						)
						)
					{
						// Es un point interior
						this->im_image_matrix[(i)-1][(j)-1] = 1;
					}
					else
					{
						// Es un point blanco
					}
				} // if (point negro)
			} // for (columnas)
		} // for (filas)

		if (this->im_debug)
			cout << " Done." << endl;

		// Actualizamos el status de la imagen y devolvemos la llamada.
		this->im_status = (t_status) ((int) this->im_status | (int) STATUS_BORDER_BLACK_COMPONENTS);
		return true;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////// medial_axis_transformation
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: Cierto
	//
	// POST: Devuelve la matriz con los puntos etiquetados en funcion de su distance
	//       al borde mas cercano (la distance dependera de la adjacence indicada).
	//
	bool image::medial_axis_transformation (
		const t_adjacence adjacence)
	{
		if ((this->im_status & STATUS_NULL) != 0)
		{
			cerr << "[ERROR]::image::medial_axis_transformation()::NULL image." << endl;
			return false;
		}

		if (this->im_debug)
			cout << "   Applying MAT... ";

		// Variables que se usaran
		dword i;
		dword j;
		byte k;
		t_colour adjacents[8];
		bool found;
		bool is_first;
		byte minor;
		t_colour major;

		if (this->im_debug)
			cout << "step 1... ";

		// Primera pasada
		for (i = 2;
			 i <= this->im_height - 1;
			 i++)
		{
			for (j = 2;
				 j <= this->im_width - 1;
				 j++)
			{
				if (this->im_image_matrix[(i)-1][(j)-1] > 0)
				{
					// Punto negro
					// Busca la minor de las components
					//     2 0 3
					//     1 x
					adjacents [0] = this->im_image_matrix[(i-1)-1][(j)-1];
					adjacents [1] = this->im_image_matrix[(i)-1][(j-1)-1];
					if (adjacence == ADJ_8B_4W)
					{
						adjacents [2] = this->im_image_matrix[(i-1)-1][(j-1)-1];
						adjacents [3] = this->im_image_matrix[(i-1)-1][(j+1)-1];
					}
					is_first = true;
					found = false;
					for (k = 0;
						 k < 4;
						 k++)
					{
						if (is_first || adjacents[k] < adjacents[minor])
						{
							if (k < 2 || adjacence == ADJ_8B_4W)
							{
								minor = k;
								is_first = false;
								found = true;
							}
						}
					}
					this->im_image_matrix[(i)-1][(j)-1] = adjacents [minor] + 1;
				} // if (point negro)
			} // for (columnas)
		} // for (filas)

		if (this->im_debug)
			cout << "step 2... ";

		// Segunda pasada
		major = 0;
		for (i = this->im_height - 1;
			 i >= 2;
			 i--)
		{
			for (j = this->im_width - 1;
				 j >= 2;
				 j--)
			{
				if (this->im_image_matrix[(i)-1][(j)-1] > 0)
				{
					// Punto negro
					// Busca la minor de las components
					//     7 3 6
					//     2 x 1
					//     5 0 4
					adjacents [0] = this->im_image_matrix[(i+1)-1][(j)-1];
					adjacents [1] = this->im_image_matrix[(i)-1][(j+1)-1];
					adjacents [2] = this->im_image_matrix[(i)-1][(j-1)-1];
					adjacents [3] = this->im_image_matrix[(i-1)-1][(j)-1];
					if (adjacence == ADJ_8B_4W)
					{
						adjacents [4] = this->im_image_matrix[(i+1)-1][(j+1)-1];
						adjacents [5] = this->im_image_matrix[(i+1)-1][(j-1)-1];
						adjacents [6] = this->im_image_matrix[(i-1)-1][(j+1)-1];
						adjacents [7] = this->im_image_matrix[(i-1)-1][(j-1)-1];
					}
					is_first = true;
					found = false;
					for (k = 0;
						 k < 8;
						 k++)
					{
						if (is_first || adjacents[k] < adjacents[minor])
						{
							if (k < 4 || adjacence == ADJ_8B_4W)
							{
								minor = k;
								is_first = false;
								found = true;
							}
						}
					}
					this->im_image_matrix[(i)-1][(j)-1] = adjacents [minor] + 1;
					if (this->im_image_matrix[(i)-1][(j)-1] > major)
						major = this->im_image_matrix[(i)-1][(j)-1];
				} // if (point negro)
			} // for (columnas)
		} // for (filas)

		this->im_black_components = (dword) major;
		this->im_status = (t_status) ((int) this->im_status | (int) STATUS_MAT);

		if (this->im_debug)
			cout << " Done." << endl;

		return true;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////////////// save
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: Cierto.
	//
	// POST: Guarda la imagen en un file BMP con las siguientes caracteristicas:
	//          - sin compression: BI_RGB
	//          - 24 bpp
	//
	bool image::save (
		const std::string filename,
		const t_save_format output_format)
	{
		if ((im_status & STATUS_NULL) != 0)
		{
			cerr << "[ERROR]::image::save()::NULL image." << endl;
			return false;
		}

		// Variables que se usaran
		fstream file;
		// Para la cabecera
		word id;
		dword file_size;
		dword reserved;
		dword data_offset;
		dword header_size;
		dword width;
		dword heigth;
		word colour_planes;
		word bpp;
		dword compression;
		dword data_size;
		dword horizontal_resolution;
		dword vertical_resolution;
		dword offset;
		char padding_char;
		// Para recorrer la matriz
		dword i;
		dword j;
		dword colour;
		t_ma_point ma_point;
		t_point simple_point;
		// Para recorrer la lista de components
		t_components_set::iterator iterator1;
		t_components_set::iterator iterator2;

		// Relleno de las filas para que el numero de bytes por fila sea multiplo de 4.
		offset = ((this->im_width*3) % 4);
		if (offset > 0)
			offset = 4 - offset;

		padding_char = 0;

		// Genera la cabecera
		id = (word) 19778;
		// Tamano de la cabecera
		file_size = 54;
		// Tamano de los puntos a almacenar (width x alto x (3 bytes/point))
		file_size += im_height * this->im_width * 3;
		// Tamano del relleno de las filas (el numero de bytes en cada fila
		// debe ser multiplo de 4.
		file_size += offset * this->im_height;
		// reserved = JCJS
		reserved = ((dword) 'J') * 0x1FFFFFF + ((dword) 'C') * 0x1FFFF + ((dword) 'J') * 0x1FF + ((dword) 'S');
		// Inicio de los datos: tras la cabecera.
		data_offset = 54;
		// Tamano de la cabecera
		header_size = 40;
		// Dimensiones de la imagen.
		width = this->im_width;
		heigth = this->im_height;
		// Informacion del colour.
		colour_planes = 1;
		bpp = 24;
		// Formato sin compression.
		compression = BI_RGB;
		// Tamano de los datos.
		data_size = file_size - 54;
		// El resto de los campos no se rellenan.
		horizontal_resolution = 0;
		vertical_resolution = 0;

		// Crea el file
		file.open (filename.c_str(), ios_base::binary | ios_base::out | ios_base::trunc );

		// Almacena la cabecera del file
		write_word_lendian (file, id);
		write_dword_lendian (file, file_size);
		write_dword_lendian (file, reserved);
		write_dword_lendian (file, data_offset);
		write_dword_lendian (file, header_size);
		write_dword_lendian (file, width);
		write_dword_lendian (file, heigth);
		write_word_lendian (file, colour_planes);
		write_word_lendian (file, bpp);
		write_dword_lendian (file, compression);
		write_dword_lendian (file, data_size);
		write_dword_lendian (file, horizontal_resolution);
		write_dword_lendian (file, vertical_resolution);
		write_dword_lendian (file, 0);
		write_dword_lendian (file, 0);

		// Almacena la matriz por filas (n..1) y despues por columnas (1..m)
		for (i = this->im_height;
			 i >= 1;
			 i--)
		{
			for (j = 1;
				 j <= this->im_width;
				 j++)
			{
				// Almacena los colores del point: azul, verde y rojo
				switch (output_format)
				{
				case SAVE_BW:
					if (this->im_image_matrix[(i)-1][(j)-1] > 0)
						colour = 0;
					else
						colour = 255;
					write_byte (file, colour);
					write_byte (file, colour);
					write_byte (file, colour);
					break;
				case SAVE_COMPONENTS:
					//if ((im_status & (STATUS_BLACK_COMPONENTS | STATUS_WHITE_COMPONENTS)) != 0)
					if (true)
					{
						// Si se han buscado components blancas o negras podemos usar este tipo
						// de salida
						if (this->im_image_matrix[(i)-1][(j)-1] > 0)
						{
							// component negra
							colour = ((255 * 3) / this->im_black_components) * this->im_image_matrix[(i)-1][(j)-1];
							if (colour < 256)
							{
								// Le corresponde un tono de azul
								write_byte (file, colour - 0);
								write_byte (file, 0);
								write_byte (file, 0);
							}
							else if (colour < 512)
							{
								// Le corresponde un tono de verde
								write_byte (file, 32);
								write_byte (file, colour - 255);
								write_byte (file, 128);
							}
							else
							{
								// Le corresponde un tono de rojo
								write_byte (file, 128);
								write_byte (file, 32);
								write_byte (file, colour - 511);
							}
						}
						else
						{
							// component blanca
							// De momento, como no funciona "buscar components blancas",
							// almacenamos un point blanco.
							write_byte (file, 255);
							write_byte (file, 255);
							write_byte (file, 255);
						}
					}
					else
					{
						// Usamos la salida por defecto (SAVE_BW).
						if (this->im_image_matrix[(i)-1][(j)-1] > 0)
							colour = 0;
						else
							colour = 255;
						write_byte (file, colour);
						write_byte (file, colour);
						write_byte (file, colour);
					}
					break;
				case SAVE_BORDES:
					if (this->im_image_matrix[(i)-1][(j)-1] > 0)
					{
						if (this->im_image_matrix[(i)-1][(j)-1] > 1)
							colour = 0;
						else
							colour = 192;
					}
					else
						colour = 255;
					write_byte (file, colour);
					write_byte (file, colour);
					write_byte (file, colour);
					break;
				case SAVE_MA:
					ma_point.i = i;
					ma_point.j = j;
					if (this->im_image_matrix[(i)-1][(j)-1] > 0)
					{
						if (this->im_ma.find(ma_point) != im_ma.end())
							colour = 0;
						else
							colour = 192;
					}
					else
						colour = 255;
					write_byte (file, colour);
					write_byte (file, colour);
					write_byte (file, colour);
					break;
				case SAVE_SIMPLE_POINTS_FIND:
					simple_point.i = i;
					simple_point.j = j;
					if (this->im_simple_points_found.find(simple_point) != this->im_simple_points_found.end())
						colour = 192;
					else
					{
						if (this->im_image_matrix[(i)-1][(j)-1] > 0)
							colour = 0;
						else
							colour = 255;
					}
					write_byte (file, colour);
					write_byte (file, colour);
					write_byte (file, colour);
					break;
				case SAVE_SIMPLE_POINTS_REMOVE:
					simple_point.i = i;
					simple_point.j = j;
					if (this->im_simple_points_removed.find(simple_point) != this->im_simple_points_removed.end())
						colour = 192;
					else
					{
						if (this->im_image_matrix[(i)-1][(j)-1] > 0)
							colour = 0;
						else
							colour = 255;
					}
					write_byte (file, colour);
					write_byte (file, colour);
					write_byte (file, colour);
					break;
				default :
					if (this->im_image_matrix[(i)-1][(j)-1] > 0)
						colour = 0;
					else
						colour = 255;
					write_byte (file, colour);
					write_byte (file, colour);
					write_byte (file, colour);
					break;
				}
			}
			// Almacena el relleno de la fila
			if (offset > 0)
				file.write (&padding_char, offset);
		}

		// Cerramos el file
		file.close();

		// Devolvemos la llamada
		return true;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////////////// ma
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: El status de la imagen debe ser STATUS_MAT.
	//
	// POST: Genera el MA: la lista de puntos cuyo valor MAT es maximo local.
	// 
	bool image::ma (
		const t_adjacence adjacence)
	{
		// El status de la imagen debe ser STATUS_MAT.
		if ((this->im_status & STATUS_MAT) == 0)
		{
			if (this->im_debug)
                cerr << "[WARNING]::image::ma()::not STATUS_MAT" << endl;
		}

		if ((this->im_status & STATUS_MA) != 0)
		{
			if (this->im_debug)
                cerr << "[WARNING]::image::ma()::STATUS_MA" << endl;
			this->im_ma.clear();
		}

		// Variables que se usaran.
		dword i;
		dword j;
		t_ma_point point;

		t_ma_set::iterator iterador;

		if (this->im_debug)
			cout << "   Calculating MA of the image with MAT data... ";

		// Recorremos la matriz
		for (i = 2;
			 i <= this->im_height - 1;
			 i++)
		{
			for (j = 2;
				 j <= this->im_width - 1;
				 j++)
			{
				if (this->im_image_matrix[(i)-1][(j)-1] > 0)
				{
					// Punto negro
					//
					//     4 0 5
					//     1 x 2
					//     6 3 7
					//
					if (this->im_image_matrix[(i)-1][(j)-1] >= this->im_image_matrix[(i-1)-1][(j)-1] &&
						this->im_image_matrix[(i)-1][(j)-1] >= this->im_image_matrix[(i)-1][(j-1)-1] &&
						this->im_image_matrix[(i)-1][(j)-1] >= this->im_image_matrix[(i)-1][(j+1)-1] &&
						this->im_image_matrix[(i)-1][(j)-1] >= this->im_image_matrix[(i+1)-1][(j)-1] &&
							(adjacence == ADJ_4B_8W ||
								(this->im_image_matrix[(i)-1][(j)-1] >= this->im_image_matrix[(i-1)-1][(j-1)-1] &&
								 this->im_image_matrix[(i)-1][(j)-1] >= this->im_image_matrix[(i-1)-1][(j+1)-1] &&
								 this->im_image_matrix[(i)-1][(j)-1] >= this->im_image_matrix[(i+1)-1][(j-1)-1] &&
								 this->im_image_matrix[(i)-1][(j)-1] >= this->im_image_matrix[(i+1)-1][(j+1)-1])
						)
						)
					{
						// Maximo local
						point.i = i;
						point.j = j;
						point.distance = this->im_image_matrix[(i)-1][(j)-1] - 1;
						this->im_ma.insert (point);
					}
				} // if (point negro)
			} // for (filas)
		} // for (columnas)

		// Actualiza el status de la imagen.
		this->im_status = (t_status) ((int) this->im_status | (int) STATUS_MA);

		if (this->im_debug)
			cout << " Done." << endl;

		// Devuelve la llamada.
		return true;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////// rebuild_from_ma
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: Se ha construido la MA de la imagen
	//
	// POST: Reconstruye la imagen original a partir de los datos de la MA:
	//               (i, j, radio_bola)
	//
	//       Va creando bolas de diametro 'radio_bola' dependiendo de la adjacence
	//       de los puntos (rombos en el caso de 4N_8B y cuadrados en 8N_4B) y centro
	//       (i, j).
	//
	bool image::rebuild_from_ma (
		const t_adjacence adjacence)
	{
		if ((this->im_status & STATUS_MA) == 0)
		{
			cerr << "[WARNING]::image::rebuild_from_ma()::not STATUS_MA" << endl;
			return false;
		}

		// Variables que se usaran
		dword i;
		dword j;
		dword d;
		dword k;
		dword l;
		dword d_i;
		dword d_j;
		image::t_ma_set::iterator iterador;

		if (this->im_debug)
			cout << "   Rebuilding image from MA data... ";

		if (this->im_debug)
			cout << "clearing image... ";
		// Borra la imagen actual
		for (i = 1;
			 i <= this->im_height;
			 i++)
		{
			for (j = 1;
				 j <= this->im_width;
				 j++)
			{
				// Rellenamos con puntos blancos.
				this->im_image_matrix[(i)-1][(j)-1] = 0;
			}
		}

		if (this->im_debug)
			cout << "creating balls (" << (adjacence == ADJ_8B_4W? "ADJ_8B_4W": "ADJ_4B_8W") << ")... ";

		// Reconstruye la imagen original a partir de los datos de las bolas
		for (iterador = this->im_ma.begin();
			 iterador != this->im_ma.end();
			 iterador++)
		{
			d = (*iterador).distance;
			i = (*iterador).i - d;
			j = (*iterador).j - d;
			if (adjacence == ADJ_4B_8W)
			{
				// Crea la bola indicada
				//         x
				//       x x x 
				//     x x O x x    4N_8B     ej: d=2
				//       x x x
				//         x
				for (k = i;
					 k <= i+2*d;
					 k++)
				{
					for (l = j;
						 l <= j+2*d;
						 l++)
					{
						if (i>k)
							d_i = i-k;
						else
							d_i = k-i;

						if (j>l)
							d_j = j-l;
						else
							d_j = l-j;

						if (d_i + d_j <= d &&
							k <= this->im_width &&
							l <= this->im_height)
						{
							this->im_image_matrix[(k)-1][(l)-1] = 1;
						}
					}
				}
			}
			else
			{
				// Crea la bola indicada
				//     x x x x x
				//     x x x x x
				//     x x O x x    8N_4B     ej: d=2
				//     x x x x x
				//     x x x x x
				for (k = i;
					 k <= i+2*d;
					 k++)
				{
					for (l = j;
						 l <= j+2*d;
						 l++)
					{
						if (k <= this->im_width && l <= this->im_height)
							this->im_image_matrix[(k)-1][(l)-1] = 1;
					}
				}
			}
		}

        if (this->im_debug)
		    cout << " Done." << endl;
		// Devolvemos la llamada
		return true;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////////////////////// simple_points
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: Se ha creado un borde blanco en la imagen.
	//
	// POST: Busca o elimina los puntos simples.
	//           - buscar puntos simples: busca puntos simples y los anade a la lista
	//                  de puntos encontrados. Si la lista no esta vacia, la borra.
	//
	//           - eliminar puntos simples: borra los puntos de la imagen (los hace
	//                  blancos), y los anade a la lista de puntos eliminados.
	//
	//       En caso de que no encuentre puntos simples (o de error) devuelve falso.
	//
	bool image::simple_points (
		const t_simple_points_algorithm algorithm,
		const t_adjacence adjacence)
	{
		if ((this->im_status & STATUS_WHITE_FRAME) == 0)
		{
			cerr << "[WARNING]::image::simple_points()::not STATUS_WHITE_FRAME" << endl;
			return false;
		}

		switch (algorithm)
		{
		case SIMPLE_POINTS_NULL:
			break;
		case SIMPLE_POINTS_FIND_SWEEP:
			if((this->im_status & STATUS_SIMPLE_POINTS_FOUND) != 0)
			{
				if (this->im_debug)
					cerr << "[WARNING]::image::simple_points()::STATUS_SIMPLE_POINTS_FOUND" << endl;
			}
			this->im_simple_points_found.clear();
			break;
		case image::SIMPLE_POINTS_REMOVE_SWEEP:
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
		case image::SIMPLE_POINTS_REMOVE_GH89A2_ODD:
		case image::SIMPLE_POINTS_REMOVE_GH89A2_EVEN:
		case image::SIMPLE_POINTS_REMOVE_TSIN_ODD:
		case image::SIMPLE_POINTS_REMOVE_TSIN_EVEN:
			// No hay problema, si se estan eliminando mas puntos simples, simplemente se anaden a la lista
			// de puntos actualmente eliminados. Si se estan buscando puntos simples, se tocara la otra lista
			// con lo que no hay problema.
			break;
		case SIMPLE_POINTS_REMOVE_BORDERS:
			// Solo se puede aplicar si se han buscado black_components_edges anteriormente
			if ((this->im_status & STATUS_BORDER_BLACK_COMPONENTS) == 0)
			{
				cerr << "[WARNING]::image::simple_points()::not STATUS_BORDER_BLACK_COMPONENTS" << endl;
				return false;
			}
			break;
		case SIMPLE_POINTS_REMOVE_BORDERS_NO_MA:
			// Solo se puede aplicar si se han buscado black_components_edges y MA anteriormente
			if ((this->im_status & (image::STATUS_BORDER_BLACK_COMPONENTS | image::STATUS_MA)) == 0)
			{
				cerr << "[WARNING]::image::simple_points()::not STATUS_BORDER_BLACK_COMPONENTS or not STATUS_MA" << endl;
				return false;
			}
			break;
		}

		// Variables que se usaran
		dword i;
		dword j;
		t_point point;
		t_ma_point ma_point;
		bool is_simple;
		bool simple_points_found;
		t_colour **matrix;

		/*
		 * If it is a step of a parallel shrink algorithm
		 * we must create a copy of the original image
		 * to simulate one step of the algorithms.
		 */
		if (algorithm == SIMPLE_POINTS_REMOVE_NORTH_FRONTIER ||
			algorithm == SIMPLE_POINTS_REMOVE_EAST_FRONTIER ||
			algorithm == SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER ||
			algorithm == SIMPLE_POINTS_REMOVE_WEST_FRONTIER ||
			algorithm == SIMPLE_POINTS_REMOVE_RH_NORTH ||
			algorithm == SIMPLE_POINTS_REMOVE_RH_EAST ||
			algorithm == SIMPLE_POINTS_REMOVE_RH_SOUTH ||
			algorithm == SIMPLE_POINTS_REMOVE_RH_WEST ||
			algorithm == SIMPLE_POINTS_REMOVE_ROS_NORTH ||
			algorithm == SIMPLE_POINTS_REMOVE_ROS_EAST ||
			algorithm == SIMPLE_POINTS_REMOVE_ROS_SOUTH ||
			algorithm == SIMPLE_POINTS_REMOVE_ROS_WEST ||
			algorithm == SIMPLE_POINTS_REMOVE_GH89A1_ODD ||
			algorithm == SIMPLE_POINTS_REMOVE_GH89A1_EVEN ||
			algorithm == SIMPLE_POINTS_REMOVE_TSIN_ODD ||
			algorithm == SIMPLE_POINTS_REMOVE_TSIN_EVEN ||
			algorithm == SIMPLE_POINTS_REMOVE_GH89A2_ODD ||
			algorithm == SIMPLE_POINTS_REMOVE_GH89A2_EVEN)
		{
			if (this->im_pshrink_initial_image_matrix)
			{
				delete this->im_pshrink_initial_image_matrix;
			}
			this->im_pshrink_initial_image_matrix = new t_colour*[this->im_height];
			for (i = 1;
				 i <= this->im_height;
				 i++)
			{
				this->im_pshrink_initial_image_matrix[(i)-1] = new t_colour[this->im_width];
				for (j = 1;
					 j <= this->im_width;
					 j++)
				{
				 this->im_pshrink_initial_image_matrix[(i)-1][(j)-1] =
					 this->im_image_matrix[(i)-1][(j)-1];
				}
			}
			matrix = this->im_pshrink_initial_image_matrix;
		}
		else
		{
			/* We are using a sequential algorithm, so we use the image matrix. */
			matrix = this->im_image_matrix;
		}

		// Recorremos la imagen y vamos buscando/eliminando los puntos simples
		simple_points_found = false;
		for (i = 1;
			 i <= this->im_height;
			 i++)
		{
			for (j = 1;
				 j <= this->im_width;
				 j++)
			{
				//    1 2 3
				//    8 x 4
				//    7 6 5
				//
				is_simple = false;
				if (matrix[(i)-1][(j)-1] <= 0)
				{
					// No es un point negro
					continue;
				}
				if (is_simple_point (matrix, i, j, adjacence))
				{
					ma_point.i = i;
					ma_point.j = j;
					// Puede haber restricciones adicionales a la de ser point simple
					switch (algorithm)
					{
					case image::SIMPLE_POINTS_REMOVE_BORDERS:
						// Ademas de ser point simple, necesitamos que sea borde.
						if (matrix[(i)-1][(j)-1] == 2)
						{
							// Es un point simple
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_BORDERS_NO_MA:
						// Ademas de ser point simple, necesitamos que sea borde y no pertenezca a la lista MA.
						if (matrix[(i)-1][(j)-1] == 2 &&
							this->im_ma.find (ma_point) == this->im_ma.end())
						{
							// Es un point simple
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_NORTH_FRONTIER:
						// Ademas de ser point simple, necesitamos que sea frontier norte y que sea adyacente
						// al menos a otros dos puntos negros.
						if (is_frontier (matrix, i, j, image::FRONTIER_NORTH/*, adjacence*/) &&
							black_adjacent_points (matrix, i,j, adjacence) >= 2)
						{
							// Es un point simple
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_EAST_FRONTIER:
						// Ademas de ser point simple, necesitamos que sea frontier norte y que sea adyacente
						// al menos a otros dos puntos negros.
						if (is_frontier (matrix, i, j, image::FRONTIER_EAST) &&
							black_adjacent_points (matrix, i,j, adjacence) >= 2)
						{
							// Es un point simple
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER:
						// Ademas de ser point simple, necesitamos que sea frontier norte y que sea adyacente
						// al menos a otros dos puntos negros.
						if (is_frontier (matrix, i, j, image::FRONTIER_SOUTH) &&
							black_adjacent_points (matrix, i,j, adjacence) >= 2)
						{
							// Es un point simple
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_WEST_FRONTIER:
						// Ademas de ser point simple, necesitamos que sea frontier norte y que sea adyacente
						// al menos a otros dos puntos negros.
						if (is_frontier (matrix, i, j, image::FRONTIER_WEST) &&
							black_adjacent_points (matrix, i,j, adjacence) >= 2)
						{
							// Es un point simple
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_RH_NORTH:
						if (image::meets_shrink_rh_conditions (i, j, image::FRONTIER_NORTH))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_RH_EAST:
						if (image::meets_shrink_rh_conditions (i, j, image::FRONTIER_EAST))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_RH_SOUTH:
						if (image::meets_shrink_rh_conditions (i, j, image::FRONTIER_SOUTH))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_RH_WEST:
						if (image::meets_shrink_rh_conditions (i, j, image::FRONTIER_WEST))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_ROS_NORTH:
						if (image::meets_shrink_ros_conditions (i, j, image::FRONTIER_NORTH))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_ROS_EAST:
						if (image::meets_shrink_ros_conditions (i, j, image::FRONTIER_EAST))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_ROS_SOUTH:
						if (image::meets_shrink_ros_conditions (i, j, image::FRONTIER_SOUTH))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_ROS_WEST:
						if (image::meets_shrink_ros_conditions (i, j, image::FRONTIER_WEST))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_GH89A1_ODD:
						if (image::meets_shrink_gh89a1_conditions (i, j, 1))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_GH89A1_EVEN:
						if (image::meets_shrink_gh89a1_conditions (i, j, 2))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_TSIN_ODD:
						if (image::meets_shrink_tsin_conditions (i, j, 1))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_TSIN_EVEN:
						if (image::meets_shrink_tsin_conditions (i, j, 2))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					case image::SIMPLE_POINTS_REMOVE_GH89A2_ODD:
					case image::SIMPLE_POINTS_REMOVE_GH89A2_EVEN:
						if (image::meets_shrink_gh89a2_conditions (i, j))
						{
							is_simple = true;
							simple_points_found = true;
						}
						break;
					default:
						// Solo exigimos que sea point simple
						is_simple = true;
						simple_points_found = true;
						break;
					}
				} // if (simple_point)
				if (is_simple)
				{
					switch (algorithm)
					{
					case SIMPLE_POINTS_NULL:
						break;
					case SIMPLE_POINTS_FIND_SWEEP:
						// Buscamos los puntos simples en una pasada.
						// Lo anadimos a la lista de puntos simples encontrados.
						point.i = i;
						point.j = j;
						this->im_simple_points_found.insert (point);
						break;
					case SIMPLE_POINTS_REMOVE_SWEEP:
					case SIMPLE_POINTS_REMOVE_BORDERS:
					case SIMPLE_POINTS_REMOVE_BORDERS_NO_MA:
					case SIMPLE_POINTS_REMOVE_NORTH_FRONTIER:
					case SIMPLE_POINTS_REMOVE_EAST_FRONTIER:
					case SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER:
					case SIMPLE_POINTS_REMOVE_WEST_FRONTIER:
					case SIMPLE_POINTS_REMOVE_RH_NORTH:
					case SIMPLE_POINTS_REMOVE_RH_EAST:
					case SIMPLE_POINTS_REMOVE_RH_SOUTH:
					case SIMPLE_POINTS_REMOVE_RH_WEST:
					case SIMPLE_POINTS_REMOVE_ROS_NORTH:
					case SIMPLE_POINTS_REMOVE_ROS_EAST:
					case SIMPLE_POINTS_REMOVE_ROS_SOUTH:
					case SIMPLE_POINTS_REMOVE_ROS_WEST:
					case SIMPLE_POINTS_REMOVE_GH89A1_ODD:
					case SIMPLE_POINTS_REMOVE_GH89A1_EVEN:
					case SIMPLE_POINTS_REMOVE_TSIN_ODD:
					case SIMPLE_POINTS_REMOVE_TSIN_EVEN:
					case SIMPLE_POINTS_REMOVE_GH89A2_ODD:
					case SIMPLE_POINTS_REMOVE_GH89A2_EVEN:
						// Eliminamos los puntos simples en cada pasada.
						// Lo anadimos a la lista de puntos simples eliminados.
						point.i = i;
						point.j = j;
						if (this->im_simple_points_removed.find (point) == this->im_simple_points_removed.end())
						{
							this->im_simple_points_removed.insert (point);
						}
						// Lo eliminamos de la imagen.
						this->im_image_matrix[(i)-1][(j)-1] = 0;
						break;
					}
				} // if (is_simple)

			} // for (columnas)
		} // for (filas)

		// Actualizamos el status de la imagen y devolvemos la llamada
		switch (algorithm)
		{
		case SIMPLE_POINTS_NULL:
			break;
		case SIMPLE_POINTS_FIND_SWEEP:
			this->im_status = (t_status) ((int) this->im_status | (int) STATUS_SIMPLE_POINTS_FOUND);
			break;
		case SIMPLE_POINTS_REMOVE_BORDERS:
		case SIMPLE_POINTS_REMOVE_BORDERS_NO_MA:
		case SIMPLE_POINTS_REMOVE_SWEEP:
		case SIMPLE_POINTS_REMOVE_NORTH_FRONTIER:
		case SIMPLE_POINTS_REMOVE_EAST_FRONTIER:
		case SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER:
		case SIMPLE_POINTS_REMOVE_WEST_FRONTIER:
		case SIMPLE_POINTS_REMOVE_RH_NORTH:
		case SIMPLE_POINTS_REMOVE_RH_EAST:
		case SIMPLE_POINTS_REMOVE_RH_SOUTH:
		case SIMPLE_POINTS_REMOVE_RH_WEST:
		case SIMPLE_POINTS_REMOVE_ROS_NORTH:
		case SIMPLE_POINTS_REMOVE_ROS_EAST:
		case SIMPLE_POINTS_REMOVE_ROS_SOUTH:
		case SIMPLE_POINTS_REMOVE_ROS_WEST:
		case SIMPLE_POINTS_REMOVE_GH89A1_ODD:
		case SIMPLE_POINTS_REMOVE_GH89A1_EVEN:
		case SIMPLE_POINTS_REMOVE_TSIN_ODD:
		case SIMPLE_POINTS_REMOVE_TSIN_EVEN:
		case SIMPLE_POINTS_REMOVE_GH89A2_ODD:
		case SIMPLE_POINTS_REMOVE_GH89A2_EVEN:
			this->im_status = (t_status) ((int) this->im_status | (int) STATUS_SIMPLE_POINTS_REMOVED);
			break;
		
		}
		return simple_points_found;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////// sequentiall_shrink
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: Se ha creado un borde blanco en la imagen.
	//
	// POST: Elimina secuencialmente los puntos simples de la imagen
	//
	bool image::sequentiall_shrink (
		const t_simple_points_algorithm algorithm,
		const t_adjacence adjacence)
	{
		if ((this->im_status & image::STATUS_WHITE_FRAME) == 0)
		{
			cerr << "[WARNING]::image::simple_points()::not STATUS_WHITE_FRAME" << endl;
			return false;
		}

		if (this->im_debug)
			cout << "   Sequential shrink -> looking for simple points ";

		// Variables que se usaran
		bool simple_point_found;
		bool current_debug_mode;

		// Hacemos un bucle que dependera del tipo de eliminacion indicada:
		//  - eliminamos puntos simples con barridos
		//  - eliminamos puntos simples de los black_components_edges:
		//      - bucle:
		//         - buscamos black_components_edges
		//         - eliminamos puntos simples
		//  - eliminamos puntos simples de los black_components_edges siempre que pertenezcan a MA:
		//      - buscamos MA
		//      - bucle:
		//          - buscamos black_components_edges
		//          - eliminamos puntos simples

		// Entramos en el bucle.
		simple_point_found = true;
		// Para no print la informacion de debug en las llamadas que se
		// hacen para buscar black_components_edges
		current_debug_mode = this->im_debug;

		// Si el algoritmo es black_components_edges que no pertenezcan a MA: buscamos MA inicial.
		if (algorithm == image::SIMPLE_POINTS_REMOVE_BORDERS_NO_MA)
		{
			medial_axis_transformation (adjacence);
			ma (adjacence);
		}

		while (simple_point_found)
		{
			// Salimos si no encontramos ningun point simple
			simple_point_found = false;

			// Aplicamos el algoritmo correspondiente
			switch (algorithm)
			{
			case SIMPLE_POINTS_NULL:
			
				break;
				
			case image::SIMPLE_POINTS_FIND_SWEEP:

				simple_point_found = image::simple_points (image::SIMPLE_POINTS_FIND_SWEEP, adjacence);
				break;

			case image::SIMPLE_POINTS_REMOVE_SWEEP:

				simple_point_found = image::simple_points (image::SIMPLE_POINTS_REMOVE_SWEEP, adjacence);
				break;

			case image::SIMPLE_POINTS_REMOVE_BORDERS:

				this->im_debug = false;

				if (!image::black_components_edges (adjacence))
				{
					this->im_debug = current_debug_mode;
					break;
				}
				this->im_debug = current_debug_mode;
				if (this->im_debug)
					cout << ".";
				simple_point_found = image::simple_points (image::SIMPLE_POINTS_REMOVE_BORDERS, adjacence);
				break;

			case image::SIMPLE_POINTS_REMOVE_BORDERS_NO_MA:

				this->im_debug = false;
				if (!image::black_components_edges (adjacence))
				{
					this->im_debug = current_debug_mode;
					break;
				}
				this->im_debug = current_debug_mode;
				if (this->im_debug)
					cout << ".";
				simple_point_found = image::simple_points (image::SIMPLE_POINTS_REMOVE_BORDERS_NO_MA, adjacence);
				break;
				
			case SIMPLE_POINTS_REMOVE_NORTH_FRONTIER:
			case SIMPLE_POINTS_REMOVE_EAST_FRONTIER:
			case SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER:
			case SIMPLE_POINTS_REMOVE_WEST_FRONTIER:
				cerr << "[ERROR]::sequential_shrink::parallel FRONTIERS algorithm found." << endl;
				break;
	
			case SIMPLE_POINTS_REMOVE_RH_NORTH:
			case SIMPLE_POINTS_REMOVE_RH_EAST:
			case SIMPLE_POINTS_REMOVE_RH_SOUTH:
			case SIMPLE_POINTS_REMOVE_RH_WEST:
				cerr << "[ERROR]::sequential_shrink::parallel RH algorithm found." << endl;
				break;

			case SIMPLE_POINTS_REMOVE_ROS_NORTH:
			case SIMPLE_POINTS_REMOVE_ROS_EAST:
			case SIMPLE_POINTS_REMOVE_ROS_SOUTH:
			case SIMPLE_POINTS_REMOVE_ROS_WEST:
				cerr << "[ERROR]::sequential_shrink::parallel RH algorithm found." << endl;
				break;
			
			case SIMPLE_POINTS_REMOVE_GH89A1_ODD:
			case SIMPLE_POINTS_REMOVE_GH89A1_EVEN:
				cerr << "[ERROR]::sequential_shrink::parallel GH89A1 algorithm found." << endl;
				break;
			
			case SIMPLE_POINTS_REMOVE_TSIN_ODD:
			case SIMPLE_POINTS_REMOVE_TSIN_EVEN:
				cerr << "[ERROR]::sequential_shrink::parallel TSIN algorithm found." << endl;
				break;
			
			case SIMPLE_POINTS_REMOVE_GH89A2_ODD:
			case SIMPLE_POINTS_REMOVE_GH89A2_EVEN:
				cerr << "[ERROR]::sequential_shrink::GH89A2 algorithm." << endl;
				break;
			
			}
		}

		if (this->im_debug)
			cout << " Done." << endl;

		return true;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////// parallel_shrink
	// /////////////////////////////////////////////////////////////////////////////
	bool image::parallel_shrink (
		const t_parallel_shrink_algorithm algorithm,
		const word num_threads)
	{
        if (this->im_debug)
		    cout << "parallel_shrink::start" << endl;
		// Variables que se usaran.
		bool do_algorithm;
		bool simple_point_found;
		word i;

        /* These are for critical sections in access to 'cout' */
		t_lock lock_cout (this->im_mutex_io, false);

		// Debemos repartir la matriz entre los distintos procesos.
		// La particion se hace por filas completas, y en caso de que haya menos
		// filas que procesos, solo se lanza un proceso por fila.

		// Como los procesos deben aplicar diferentes partes del algoritmo de forma concurrente,
		// pero se deben sincronizar antes de iniciar un nuevo proceso (los algoritmos
		// requieren que los distintos pasos no sean concurrentes, aunque en cada paso
		// se pueda aplicar eliminacion paralela).

		// Iniciamos la estructura de datos de las acciones que llevaran a cabo
		// los hilos de ejecucion.
		pshrink_init_structure (algorithm, num_threads);

		// We create the threads.
        if (this->im_debug)
        {
		    lock_cout.lock();
		    cout << "parallel_shrink::create_threads, num_threads = " << num_threads << endl;
		    lock_cout.unlock();
        }
		for (i = 1;
			 i <= num_threads;
			 i++)
		{
			//cout << "parallel_shrink::create_threads::thread(" << i << ")" << endl;
			this->pshrink_threads_group.create_thread (boost::bind (&this->pshrink_threads_main, this, i));
		}

		do_algorithm = true;
		// Bucle de los pasos del algoritmo
		while (do_algorithm)
		{
			// Preparamos las acciones que debera realizar cada proceso.
			pshrink_set_actions ();

			/* The last thread which finishes the work will notify us */
			this->im_threads_actions.threads_working = this->im_threads_actions.num_threads;

            if (this->im_debug)
            {
			    lock_cout.lock();
			    cout << "parallel_shrink::wait_orders::notify_all" << endl;
			    lock_cout.unlock();
            }
            // The threads will start to play.
			this->im_barrier_wait_orders->wait();

            if (this->im_debug)
            {
			    lock_cout.lock();
			    cout << "parallel_shrink::wait_competion::wait" << endl;
			    lock_cout.unlock();
            }
            // We wait until the threads finishes.
			this->im_barrier_wait_completion->wait ();

            if (this->im_debug)
            {
			    lock_cout.lock();
			    cout << "parallel_shrink::wait_competion::continue" << endl;
			    lock_cout.unlock();
            }

			// Now we determine if we have finished the work.
			// If we have completed the algorithm (all the stages) and we haven't found
			// any simple point, then we exit.
			simple_point_found = false;
			for (i = 1;
				 i <= num_threads;
				 i++)
			{
				simple_point_found = simple_point_found ||
					this->im_threads_actions.thread[(i)-1].simple_point_found;
			}
            if (this->im_debug)
            {
			    if (simple_point_found)
			    {
				    lock_cout.lock();
				    cout << "parallel_shrink::simple_point_found" << endl;
				    lock_cout.unlock();
                }
			}
			/* Have we found simple points in the past stages of the current iteration of the algorithm ? */
			this->im_threads_actions.simple_points_found = this->im_threads_actions.simple_points_found || simple_point_found;
			/*
			 * We determine if we must continue with the algorithm,
			 * or if we must exit because we have finished.
			 */
			do_algorithm = pshrink_continue_condition();
			/*
			 * If we are stepping to the next iteration (no more stages
			 * of the current), we must update the matrix with the
			 * changes made.
			 */
			if (pshrink_test_if_iteration_end())
			{
                if (this->im_debug)
                {
				    lock_cout.lock();
				    cout << "parallel_shrink::end_iteration::update_matrix()" << endl;
				    lock_cout.unlock();
                }
				pshrink_end_iteration_update_matrix();
			}
			/*
			 * Now we jump to the next stage of the algorithm
			 */
			if (do_algorithm)
			{
                if (this->im_debug)
                {
				    lock_cout.lock();
				    cout << "parallel_shrink::next_stage" << endl;
				    lock_cout.unlock();
                }
				pshrink_algorithm_next_stage();
			}
		} // while (do_algorithm)
		// We force the threads to exit.
		for (i = 1;
			 i <= this->im_threads_actions.num_threads;
			 i++)
		{
			this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_NULL;
		}

		// Abrimos el mutex para que empiecen a ejecutar todos los procesos.
        if (this->im_debug)
        {
		    lock_cout.lock();
		    cout << "parallel_shrink::telling_threads_to_exit" << endl;
		    cout << "parallel_shrink::wait_orders::notify" << endl;
		    lock_cout.unlock();
        }
		this->im_barrier_wait_orders->wait();

		// Esperamos a los procesos.
		// Aqui finaliza la ejecucion de los procesos.
        if (this->im_debug)
        {
		    lock_cout.lock();
		    cout << "parallel_shrink::threads::join_all" << endl;
		    lock_cout.unlock();
        }
		this->pshrink_threads_group.join_all();

		// Destruimos el grupo de hilos de ejecucion.
		delete this->im_threads_actions.thread;

        // We destroy the barrier of synchronization
        delete this->im_barrier_wait_completion;
        delete this->im_barrier_wait_orders;

		// Devolvemos la llamada.
		return true;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////// pshrink_end_iteration_update_matrix
	// /////////////////////////////////////////////////////////////////////////////
	void image::pshrink_end_iteration_update_matrix(void)
	{
		/*
		 * Variables 
		 */
		t_points_set::iterator iterator;

		/*
		 * In parallel shrink algorithms, the conditions of removing a point
		 * are based in an untouched initial image matrix, but, after
		 * each iteration, the initial matrix must be updated with the changes.
		 */
		for (iterator = this->im_simple_points_removed.begin();
			 iterator != this->im_simple_points_removed.end();
			 iterator++)
		{
			/* For each point removed, we update its state to the matrix
			 * used for the conditions
			 */
			this->im_pshrink_initial_image_matrix[(*iterator).i-1][(*iterator).j-1] = 0;
		}
	}
	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////// pshrink_test_if_iteration_end
	// /////////////////////////////////////////////////////////////////////////////
	bool image::pshrink_test_if_iteration_end(void)
	{
		/*
		 * Variables.
		 */
		t_lock lock_cout (this->im_mutex_io, false);

		/*
		 * Returns if the last stage of an iteration has finished.
		 */
		switch (this->im_threads_actions.algorithm)
		{
        case image::PARALLEL_SHRINK_ALGORITHM_NULL:
            if (this->im_debug)
            {
                lock_cout.lock();
                cerr << "pshrink_test_if_iteration_end::[ERROR]::NULL_algorithm_found" << endl;
                lock_cout.unlock();
            }
            break;

		case image::PARALLEL_SHRINK_ALGORITHM_FRONTIERS:
			return (this->im_threads_actions.stage % 4) == 0;
			break;	

		case image::PARALLEL_SHRINK_ALGORITHM_RH:
			return (this->im_threads_actions.stage % 1) == 0;
			break;

		case image::PARALLEL_SHRINK_ALGORITHM_ROS:
			return (this->im_threads_actions.stage % 1) == 0;
			break;

		case image::PARALLEL_SHRINK_ALGORITHM_GH89A1:
			return (this->im_threads_actions.stage % 1) == 0;
			break;

		case image::PARALLEL_SHRINK_ALGORITHM_TSIN:
			return (this->im_threads_actions.stage % 1) == 0;
            break;

		case image::PARALLEL_SHRINK_ALGORITHM_GH89A2:
			return (this->im_threads_actions.stage % 1) == 0;
			break;
		}
		/* Should not run */
		return false;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////////////// black_adjacent_points
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: Cierto.
	//
	// POST: Devuelve el numero de puntos negros adjacents al point dado.
	//
	image::byte image::black_adjacent_points (
		const dword i,
		const dword j,
		const t_adjacence adjacence)
	{
		return black_adjacent_points (this->im_image_matrix, i, j, adjacence);
	}
	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////////////// black_adjacent_points
	// /////////////////////////////////////////////////////////////////////////////
	image::byte image::black_adjacent_points (
		t_colour ** matrix,
		const dword i,
		const dword j,
		const t_adjacence adjacence)
	{
		// Variables que se usaran.
		byte counter;

		// Comprobamos que el point este dentro del rango heigth x width.
		if (i > this->im_height ||
			j > this->im_width)
		{
            cerr << "[ERROR]::image::black_adjacent_points()::point (i=" << i << ",j=" << j <<") out of range ";
			cerr << "(1.." << this->im_height << ", 1.." << this->im_width << ")" << endl;
			return 0;
		}

		counter = 0;
		//
		//  5 1 6
		//  2 x 3
		//  7 4 8
		//
		if (i > 1 &&
			matrix[(i-1)-1][(j)-1] > 0)
		{
			counter++;
		}

		if (j > 1 &&
			matrix[(i)-1][(j-1)-1] > 0)
		{
			counter++;
		}

		if (j < this->im_width &&
			matrix[(i)-1][(j+1)-1] > 0)
		{
			counter++;
		}

		if (i < this->im_height &&
			matrix[(i+1)-1][(j)-1] > 0)
		{
			counter++;
		}

		if (adjacence == image::ADJ_8B_4W &&
			i > 1 &&
			j > 1 &&
			matrix[(i-1)-1][(j-1)-1] > 0)
		{
			counter++;
		}

		if (adjacence == image::ADJ_8B_4W &&
			i > 1 &&
			j < this->im_width &&
			matrix[(i-1)-1][(j+1)-1] > 0)
		{
			counter++;
		}

		if (adjacence == image::ADJ_8B_4W &&
			i < this->im_height &&
			j > 1 &&
			matrix[(i+1)-1][(j-1)-1] > 0)
		{
			counter++;
		}

		if (adjacence == image::ADJ_8B_4W &&
			i < this->im_height &&
			j < this->im_width &&
			matrix[(i+1)-1][(j+1)-1] > 0)
		{
			counter++;
		}

		return counter;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////////// is_frontier
	// /////////////////////////////////////////////////////////////////////////////
	bool image::is_frontier (
		const dword i,
		const dword j,
		const t_frontier frontier)
	{
		return is_frontier (this->im_image_matrix, i, j, frontier);
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////////// is_frontier
	// /////////////////////////////////////////////////////////////////////////////
	bool image::is_frontier (
		image::t_colour **matrix,
		const dword i,
		const dword j,
		const t_frontier frontier)
	{
		// Variables que se usaran
		bool result;

		// Precondicion
		if (i < 2 || i >= this->im_height ||
			j < 2 || j >= this->im_width)
		{
            cerr << "[ERROR]::image::is_frontier()::point (i=" << i << ",j=" << j <<") out of range ";
			cerr << "(2.." << this->im_height-1 << ", 2.." << this->im_width-1 << ")" << endl;
			return false;
		}

		result = false;
		switch (frontier)
		{
		case image::FRONTIER_NORTH:
			//     0
			//     x
			//
			if (matrix[(i-1)-1][(j)-1] <= 0)
			{
				result = true;
			}
			break;

		case image::FRONTIER_EAST:
			//
			//     x 0
			//
			if (matrix[(i)-1][(j+1)-1] <= 0)
			{
				result = true;
			}
			break;

		case image::FRONTIER_SOUTH:
			//
			//     x
			//     0
			if (matrix[(i+1)-1][(j)-1] <= 0)
			{
				result = true;
			}
			break;

		case image::FRONTIER_WEST:
			//
			//   0 x
			//
			if (matrix[(i)-1][(j-1)-1] <= 0)
			{
				result = true;
			}
			break;
		}
		// Devolvemos el valor.
		return result;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////// pshrink_init_structure
	// /////////////////////////////////////////////////////////////////////////////
	//
	bool image::pshrink_init_structure (
		const t_parallel_shrink_algorithm algorithm,
		const word num_threads)
	{
        if (this->im_debug)
		    cout << "pshrink_thread_init_structure::start" << endl;
		// Variables
		dword rows_per_thread;
		dword num_of_threads;
		dword i;
		dword j;
		bool search;

		/*
		 * We clear the simple points removed list.
		 * This is because in parallel shrink, the conditions that
		 * makes the algorithm remove a point, are based in an untouched
		 * original image (a copy of the original matrix made before
		 * the algorithm starts), so we need to know when a "new"
		 * simple point has been found (in reference to past iterations
		 * of the algorithm). Then, when removing a simple point from
		 * the image, it is added to the simple points removed list, and
		 * in the next iterations, in which the point will still be in
		 * the original untouched matrix (in which the condition will be
		 * evaluated again), we will not try to remove again the same
		 * point, by looking for it in the list.
		 */
		this->im_simple_points_removed.clear();

		// We copy the values
		this->im_threads_actions.num_threads = num_threads;
		this->im_threads_actions.adjacence = image::ADJ_8B_4W;
		this->im_threads_actions.algorithm = algorithm;
		this->im_threads_actions.stage = 1;
		this->im_threads_actions.simple_points_found = false;

		// We copy the original matrix.
        if (this->im_debug)
		    cout << "pshrink_thread_init_structure::copy_matrix(" << this->im_height << "," << this->im_width << ")" << endl;
		if (this->im_pshrink_initial_image_matrix)
		{
			delete this->im_pshrink_initial_image_matrix;
		}
		this->im_pshrink_initial_image_matrix = new t_colour*[this->im_height];
		for (i = 1;
			 i <= this->im_height;
			 i++)
		{
            this->im_pshrink_initial_image_matrix[(i)-1] = new t_colour[this->im_width];
			for (j = 1;
				 j <= this->im_width;
				 j++)
			{
				this->im_pshrink_initial_image_matrix[(i)-1][(j)-1] =
					this->im_image_matrix[(i)-1][(j)-1];
			}
		}
		
		// We need at least 4 rows per thread, so we are going to find
		// the maximum number of threads we need (decreasing the original
		// number of threads given if there are not enought rows in the matrix).
        if (this->im_debug)
		    cout << "pshrink_thread_init_structure::set_number_of_threads" << endl;
		num_of_threads = num_threads;
		search = true;
		while (search)
		{
			rows_per_thread = this->im_height-2 / num_of_threads;

			if (rows_per_thread <= 4)
			{
				// There are not enought rows to asign to the threads
				// some of them will do nothing.
				num_of_threads--;
			}
			else
			{
				// There are at least 4 rows per thread
				search = false;
			}
		} // while (search)
        if (this->im_debug)
		    cout << "pshrink_thread_init_structure::num_threads = " << num_of_threads << endl;
		// Now we have the correct number of threads.
        // We create the barriers for synchronization.
        if (this->im_debug)
            cout << "pshrink_thread_init_structure::wait_completion::barrier(" << num_of_threads << ")" << endl;
        this->im_barrier_wait_completion = new boost::barrier(num_of_threads + 1);
        if (this->im_debug)
            cout << "pshrink_thread_init_structure::wait_orders::barrier(" << num_of_threads << ")" << endl;
        this->im_barrier_wait_orders = new boost::barrier(num_of_threads + 1);
		// Now we allocate the necesary memory.
		this->im_threads_actions.thread = new t_thread_action[num_of_threads];
		for (i = 1;
			 i <= num_threads;
			 i++)
		{
			if (i <= num_of_threads)
			{
				// We assign the range of rows in which the thread will apply the
				// algorithm (the image has a white frame of at least 1 point, so the range
				// of points in which we appy the algorithm is the box inside the one point frame.
				this->im_threads_actions.thread[(i)-1].range_assigned = true;
				this->im_threads_actions.thread[(i)-1].range.start.i = (i-1) * rows_per_thread + 2;
				this->im_threads_actions.thread[(i)-1].range.start.j = 2;
				this->im_threads_actions.thread[(i)-1].range.end.i = (i) * rows_per_thread + 1;
				this->im_threads_actions.thread[(i)-1].range.end.j = this->im_width - 1;
				// We assign the correct values for the position
				this->im_threads_actions.thread[(i)-1].pos.i = this->im_threads_actions.thread[(i)-1].range.start.i;
				this->im_threads_actions.thread[(i)-1].pos.j = this->im_threads_actions.thread[(i)-1].range.start.j;
			}
			else
			{
				// This thread will do nothing.
				this->im_threads_actions.thread[(i)-1].range_assigned = false;
			}
		} // for (thread)
        if (this->im_debug)
		    cout << "pshrink_thread_init_structure::end" << endl;
		return true;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////// pshrink_set_actions
	// /////////////////////////////////////////////////////////////////////////////
	//
	void image::pshrink_set_actions (
		void)
	{
        // Variables
		word i;
        t_lock lock_cout(this->im_mutex_io, false);
        
        if (this->im_debug)
        {
            lock_cout.lock();
		    cout << "pshrink_set_actions::start" << endl;
            lock_cout.unlock();
        }

		// This set the actions that the threads must apply to the image.
		// There are several algorithms which has one or more stages.
		switch (this->im_threads_actions.algorithm)
		{
		case PARALLEL_SHRINK_ALGORITHM_NULL:
            if (this->im_debug)
            {
                lock_cout.lock();
			    cout << "pshrink_set_actions::algorithm = NULL" << endl;
                lock_cout.unlock();
            }
			break;
		
		case PARALLEL_SHRINK_ALGORITHM_FRONTIERS:
			if (this->im_threads_actions.stage == 1)
			{
                if (this->im_debug)
                {
                    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = FRONTIERS (stage 1)" << endl;
                    lock_cout.unlock();
                }
				// We are starting the algorithm: north simple points.
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					// If the thread has a range assigned.
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_NORTH_FRONTIER;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else if (this->im_threads_actions.stage == 2)
			{
                if (this->im_debug)
                {
                    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = FRONTIERS (stage 2)" << endl;
                    lock_cout.unlock();
                }
				// East simple points.
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					// If the thread has a range assigned.
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_EAST_FRONTIER;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else if (this->im_threads_actions.stage == 3)
			{
                if (this->im_debug)
                {
                    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = FRONTIERS (stage 3)" << endl;
                    lock_cout.unlock();
                }
				// South simple points.
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					// If the thread has a range assigned.
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else if (this->im_threads_actions.stage == 4)
			{
                if (this->im_debug)
                {
                    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = FRONTIERS (stage 4)" << endl;
                    lock_cout.unlock();
                }
				// West simple points.
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					// If the thread has a range assigned.
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_WEST_FRONTIER;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			break;

		case PARALLEL_SHRINK_ALGORITHM_RH:
			if (this->im_threads_actions.stage == 1)
			{
                if (this->im_debug)
                {
				    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = RH_NORTH" << endl;
				    lock_cout.unlock();
                }
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_RH_NORTH;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else if (this->im_threads_actions.stage == 2)
			{
                if (this->im_debug)
                {
				    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = RH_EAST" << endl;
				    lock_cout.unlock();
                }
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_RH_EAST;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else if (this->im_threads_actions.stage == 3)
			{
                if (this->im_debug)
                {
				    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = RH_SOUTH" << endl;
				    lock_cout.unlock();
                }
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_RH_SOUTH;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else if (this->im_threads_actions.stage == 4)
			{
                if (this->im_debug)
                {
				    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = RH_WEST" << endl;
				    lock_cout.unlock();
                }
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_RH_WEST;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			break;

		case PARALLEL_SHRINK_ALGORITHM_ROS:
			if (this->im_threads_actions.stage == 1)
			{
                if (this->im_debug)
                {
				    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = ROS_NORTH" << endl;
				    lock_cout.unlock();
                }
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_ROS_NORTH;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else if (this->im_threads_actions.stage == 2)
			{
                if (this->im_debug)
                {
				    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = ROS_EAST" << endl;
				    lock_cout.unlock();
                }
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_ROS_EAST;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else if (this->im_threads_actions.stage == 3)
			{
                if (this->im_debug)
                {
				    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = ROS_SOUTH" << endl;
				    lock_cout.unlock();
                }
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_ROS_SOUTH;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else if (this->im_threads_actions.stage == 4)
			{
                if (this->im_debug)
                {
				    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = ROS_WEST" << endl;
				    lock_cout.unlock();
                }
				for (i = 1;
					 i <= this->im_threads_actions.num_threads;
					 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_ROS_WEST;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			break;

		case PARALLEL_SHRINK_ALGORITHM_GH89A1:
			if (this->im_threads_actions.stage == 1)
			{
                if (this->im_debug)
                {
                    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = GH89A1 (odd iteration)" << endl;
                    lock_cout.unlock();
                }
				for (i = 1;
				 i <= this->im_threads_actions.num_threads;
				 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_GH89A1_ODD;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else
			{
                if (this->im_debug)
                {
                    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = GH89A1 (even iteration)" << endl;
                    lock_cout.unlock();
                }
				for (i = 1;
				 i <= this->im_threads_actions.num_threads;
				 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_GH89A1_EVEN;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			break;

		case PARALLEL_SHRINK_ALGORITHM_TSIN:
			if (this->im_threads_actions.stage == 1)
			{
                if (this->im_debug)
                {
                    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = TSIN (odd iteration)" << endl;
                    lock_cout.unlock();
                }
				for (i = 1;
				 i <= this->im_threads_actions.num_threads;
				 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_TSIN_ODD;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else
			{
                if (this->im_debug)
                {
                    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = TSIN (even iteration)" << endl;
                    lock_cout.unlock();
                }
				for (i = 1;
				 i <= this->im_threads_actions.num_threads;
				 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_TSIN_EVEN;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			break;

		case PARALLEL_SHRINK_ALGORITHM_GH89A2:
			if (this->im_threads_actions.stage == 1)
			{
                if (this->im_debug)
                {
                    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = GH89A2 (odd iteration)" << endl;
                    lock_cout.unlock();
                }
				for (i = 1;
				 i <= this->im_threads_actions.num_threads;
				 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_GH89A2_ODD;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			else
			{
                if (this->im_debug)
                {
                    lock_cout.lock();
				    cout << "pshrink_set_actions::algorithm = GH89A2 (even iteration)" << endl;
                    lock_cout.unlock();
                }
				for (i = 1;
				 i <= this->im_threads_actions.num_threads;
				 i++)
				{
					if (this->im_threads_actions.thread[(i)-1].range_assigned)
					{
						this->im_threads_actions.thread[(i)-1].algorithm = image::SIMPLE_POINTS_REMOVE_GH89A2_EVEN;
						this->im_threads_actions.thread[(i)-1].simple_point_found = false;
					}
				}
			}
			break;
		}
        if (this->im_debug)
        {
            lock_cout.lock();
		    cout << "pshrink_set_actions::end" << endl;
            lock_cout.unlock();
        }
	}

	// /////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////////////// pshrink_threads_main
	// /////////////////////////////////////////////////////////////////////////////
	//
	void image::pshrink_threads_main (
		image * object,
		word thread_number)
	{
		
		// Variables.
		bool we_must_do_things;
		bool working_in_range;
		bool remove_point;
		dword i;
		dword j;
		t_point point;

		/* We just want to create the locks, not to create and lock the mutex */
		t_lock lock_wait_orders (object->im_pshrink_mutex_wait_orders, false);
		t_lock access_lock (object->im_pshrink_mutex_access_object, false);
		t_lock lock_cout (object->im_mutex_io, false);

        if (object->im_debug)
        {
		    lock_cout.lock();
		    cout << "thread(" << thread_number << ")::start, range=(" << object->im_threads_actions.thread[thread_number-1].range.start.i << "," << object->im_threads_actions.thread[thread_number-1].range.start.j << ") -> (" << object->im_threads_actions.thread[thread_number-1].range.end.i << "," << object->im_threads_actions.thread[thread_number-1].range.end.j << "), algorithm=" << object->im_threads_actions.thread[thread_number-1].algorithm << endl;
		    lock_cout.unlock();
        }

        /* Main execution loop */
		we_must_do_things = true;
		remove_point = false;
		while (we_must_do_things)
		{
            if (object->im_debug)
            {
			    lock_cout.lock();
			    cout << "thread(" << thread_number << ")::must_do_things" << endl;
			    cout << "thread(" << thread_number << ")::wait_orders::wait" << endl;
			    lock_cout.unlock();
            }

            // We wait for an action to do.
			lock_wait_orders.lock();
			object->im_barrier_wait_orders->wait();
			lock_wait_orders.unlock();

            if (object->im_debug)
            {
			    lock_cout.lock();
			    cout << "thread(" << thread_number << ")::wait_orders::continue" << endl;
			    lock_cout.unlock();
            }

			// We do the action assigned to this thread.
			if (object->im_threads_actions.thread[(thread_number)-1].algorithm == image::SIMPLE_POINTS_NULL)
			{
				// We must exit.
                if (object->im_debug)
                {
				    lock_cout.lock();
				    cout << "thread(" << thread_number << ")::must_exit" << endl;
				    lock_cout.unlock();
                }
				we_must_do_things = false;
			}
			else
			{
				/* Now we apply the algorithm to all the points in the range assigned. */
				working_in_range = true;
				while (working_in_range)
				{
					// I don't want to write a lot.
					i = object->im_threads_actions.thread[(thread_number)-1].pos.i;
					j = object->im_threads_actions.thread[(thread_number)-1].pos.j;

					remove_point = false;
					// We apply the algorithm.
					if (object->im_pshrink_initial_image_matrix[(i)-1][(j)-1] > 0)
					{
						switch (object->im_threads_actions.thread[(thread_number)-1].algorithm)
						{
						case image::SIMPLE_POINTS_NULL:
							// They don't want us to do anything.
							working_in_range = false;
							we_must_do_things = false;
                            if (object->im_debug)
                            {
							    lock_cout.lock();
							    cout << "thread(" << thread_number << ")::wait_orders::must_exit" << endl;
							    lock_cout.unlock();
                            }
							continue;
						
						case image::SIMPLE_POINTS_FIND_SWEEP:
						case image::SIMPLE_POINTS_REMOVE_SWEEP:
						case image::SIMPLE_POINTS_REMOVE_BORDERS:
						case image::SIMPLE_POINTS_REMOVE_BORDERS_NO_MA:
							// This is a biiiiigggg error
                            if (object->im_debug)
                            {
							    lock_cout.lock();
							    cerr << "thread(" << thread_number << ")::[ERROR]:: algorithm = " << object->im_threads_actions.thread[(thread_number)-1].algorithm << " should not be here... thread " << thread_number << " exiting..." << endl;
							    lock_cout.unlock();
                            }
							working_in_range = false;
							we_must_do_things = false;
							continue;
						
						case image::SIMPLE_POINTS_REMOVE_NORTH_FRONTIER:
							if (object->is_simple_point (object->im_pshrink_initial_image_matrix, i, j, object->im_threads_actions.adjacence)
								&&
								object->is_frontier (object->im_pshrink_initial_image_matrix, i, j, image::FRONTIER_NORTH)
								&&
								object->black_adjacent_points (object->im_pshrink_initial_image_matrix, i, j, image::ADJ_8B_4W) >= 2)
							{
								remove_point = true;
							}

							break;
						
						case image::SIMPLE_POINTS_REMOVE_EAST_FRONTIER:
							if (object->is_simple_point (object->im_pshrink_initial_image_matrix, i, j, object->im_threads_actions.adjacence)
								&&
								object->is_frontier (object->im_pshrink_initial_image_matrix, i,j, image::FRONTIER_EAST)
								&&
								object->black_adjacent_points (object->im_pshrink_initial_image_matrix, i, j, image::ADJ_8B_4W) >= 2)
							{
								remove_point = true;
							}
							break;
								
						case image::SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER:
							if (object->is_simple_point (object->im_pshrink_initial_image_matrix, i, j, object->im_threads_actions.adjacence)
								&&
								object->is_frontier (object->im_pshrink_initial_image_matrix, i,j, image::FRONTIER_SOUTH)
								&&
								object->black_adjacent_points (object->im_pshrink_initial_image_matrix, i, j, image::ADJ_8B_4W) >= 2)
							{
								remove_point = true;
							}
							break;
							
						case image::SIMPLE_POINTS_REMOVE_WEST_FRONTIER:
							if (object->is_simple_point (object->im_pshrink_initial_image_matrix, i, j, object->im_threads_actions.adjacence)
								&&
								object->is_frontier (object->im_pshrink_initial_image_matrix, i,j, image::FRONTIER_WEST)
								&&
								object->black_adjacent_points (object->im_pshrink_initial_image_matrix, i, j, image::ADJ_8B_4W) >= 2)
							{
								remove_point = true;
							}
							break;

						case image::SIMPLE_POINTS_REMOVE_RH_NORTH:
							if (object->meets_shrink_rh_conditions (i, j, image::FRONTIER_NORTH))
							{
								remove_point = true;
							}
							break;

						case image::SIMPLE_POINTS_REMOVE_RH_EAST:
							if (object->meets_shrink_rh_conditions (i, j, image::FRONTIER_EAST))
							{
								remove_point = true;
							}
							break;

						case image::SIMPLE_POINTS_REMOVE_RH_SOUTH:
							if (object->meets_shrink_rh_conditions (i, j, image::FRONTIER_SOUTH))
							{
								remove_point = true;
							}
							break;

						case image::SIMPLE_POINTS_REMOVE_RH_WEST:
							if (object->meets_shrink_rh_conditions (i, j, image::FRONTIER_WEST))
							{
								remove_point = true;
							}
							break;

						case image::SIMPLE_POINTS_REMOVE_ROS_NORTH:
							if (object->meets_shrink_ros_conditions (i, j, image::FRONTIER_NORTH))
							{
								remove_point = true;
							}
							break;

						case image::SIMPLE_POINTS_REMOVE_ROS_EAST:
							if (object->meets_shrink_ros_conditions (i, j, image::FRONTIER_EAST))
							{
								remove_point = true;
							}
							break;

						case image::SIMPLE_POINTS_REMOVE_ROS_SOUTH:
							if (object->meets_shrink_ros_conditions (i, j, image::FRONTIER_SOUTH))
							{
								remove_point = true;
							}
							break;

						case image::SIMPLE_POINTS_REMOVE_ROS_WEST:
							if (object->meets_shrink_ros_conditions (i, j, image::FRONTIER_WEST))
							{
								remove_point = true;
							}
							break;
						
						case image::SIMPLE_POINTS_REMOVE_GH89A1_ODD:
							if (object->meets_shrink_gh89a1_conditions (i, j, 1))
							{
								remove_point = true;
							}
							break;

						case image::SIMPLE_POINTS_REMOVE_GH89A1_EVEN:
							if (object->meets_shrink_gh89a1_conditions (i, j, 2))
							{
								remove_point = true;
							}
							break;
							
						case image::SIMPLE_POINTS_REMOVE_TSIN_ODD:
							if (object->meets_shrink_tsin_conditions (i, j, 1))
							{
								remove_point = true;
							}
							break;

						case image::SIMPLE_POINTS_REMOVE_TSIN_EVEN:
							if (object->meets_shrink_tsin_conditions (i, j, 2))
							{
								remove_point = true;
							}
							break;
						
						case image::SIMPLE_POINTS_REMOVE_GH89A2_ODD:
							if (object->meets_shrink_gh89a2_conditions (i, j))
							{
								remove_point = true;
							}
							break;
						
						case image::SIMPLE_POINTS_REMOVE_GH89A2_EVEN:
							if (object->meets_shrink_gh89a2_conditions (i, j))
							{
								remove_point = true;
							}
							break;
						}
					} // if (black point)

					// We remove the point if we must do it
					if (remove_point)
					{
						point.i = i;
						point.j = j;
						if (object->im_simple_points_removed.find (point) == object->im_simple_points_removed.end())
						{
							/* The point has not been removed in past iterations */
							object->im_image_matrix[(i)-1][(j)-1] = 0;
							object->im_threads_actions.thread[(thread_number)-1].simple_point_found = true;
							object->im_simple_points_removed.insert (point);
						}
						else
						{
							/* I don't remember if I use this value bottom */
							remove_point = false;
						}
					}

					// We move the position to the next point in the range.
					if (object->im_threads_actions.thread[(thread_number)-1].pos.j >= object->im_width-1)
					{
						// We continue with the next row.
						object->im_threads_actions.thread[(thread_number)-1].pos.i++;
						/* And set the col to the first column */
						if (object->im_threads_actions.stage == 1)
						{
							/*
							 * x _ x _ x _ ...  <---   i mod 2 = 1
							 * _ x _ x _ x ...  <---   i mod 2 = 0
							 */
							if ((object->im_threads_actions.thread[(thread_number)-1].pos.i % 2) == 1)
								object->im_threads_actions.thread[(thread_number)-1].pos.j = 3; /* range (2..im_height) */
							else
								object->im_threads_actions.thread[(thread_number)-1].pos.j = 2; /* range (2..im_width) */
						}
						else if (object->im_threads_actions.stage == 2)
						{
							/*
							 * _ y _ y _ y ...  <---   (i+1) mod 2 = 1
							 * y _ y _ y _ ...  <---   (i+1) mod 2 = 0
							 */
							if ((object->im_threads_actions.thread[(thread_number)-1].pos.i % 2) == 1)
								object->im_threads_actions.thread[(thread_number)-1].pos.j = 2; /* range (2..im_height) */
							else
								object->im_threads_actions.thread[(thread_number)-1].pos.j = 3; /* range (2..im_width) */
						}
						else
						{
							object->im_threads_actions.thread[(thread_number)-1].pos.j = 2;
						}
					}
					else
					{
						// We continue with the next col.
						if (object->im_threads_actions.thread[(thread_number)-1].algorithm ==
										image::SIMPLE_POINTS_REMOVE_GH89A2_ODD
							||
							object->im_threads_actions.thread[(thread_number)-1].algorithm ==
										image::SIMPLE_POINTS_REMOVE_GH89A2_EVEN
							)
						{
							object->im_threads_actions.thread[(thread_number)-1].pos.j += 2;
						}
						else
						{
							object->im_threads_actions.thread[(thread_number)-1].pos.j++;
						}
					}

					// If we are out of range we must exit.
					if (	object->im_threads_actions.thread[(thread_number)-1].pos.i >
							object->im_threads_actions.thread[(thread_number)-1].range.end.i
						||
							(	object->im_threads_actions.thread[(thread_number)-1].pos.i ==
								object->im_threads_actions.thread[(thread_number)-1].range.end.i
							 &&
								object->im_threads_actions.thread[(thread_number)-1].pos.j >
								object->im_threads_actions.thread[(thread_number)-1].range.end.j
							)
						)
					{
						// We have finished with the range assigned.

						/* We reset our pos to the start of the range */
						object->im_threads_actions.thread[(thread_number)-1].pos.i =
							object->im_threads_actions.thread[(thread_number)-1].range.start.i;
						/* And set the col to the first column, acording to the next stage */
						if (object->im_threads_actions.stage == 2)
						{
							/*
							 * x _ x _ x _ ...  <---   i mod 2 = 1
							 * _ x _ x _ x ...  <---   i mod 2 = 0
							 */
							if ((object->im_threads_actions.thread[(thread_number)-1].pos.i % 2) == 1)
								object->im_threads_actions.thread[(thread_number)-1].pos.j = 3; /* range (2..im_height) */
							else
								object->im_threads_actions.thread[(thread_number)-1].pos.j = 2; /* range (2..im_width) */
						}
						else if (object->im_threads_actions.stage == 1)
						{
							/*
							 * _ y _ y _ y ...  <---   (i+1) mod 2 = 1
							 * y _ y _ y _ ...  <---   (i+1) mod 2 = 0
							 */
							if ((object->im_threads_actions.thread[(thread_number)-1].pos.i % 2) == 1)
								object->im_threads_actions.thread[(thread_number)-1].pos.j = 2; /* range (2..im_height) */
							else
								object->im_threads_actions.thread[(thread_number)-1].pos.j = 3; /* range (2..im_width) */
						}
						else
						{
							object->im_threads_actions.thread[(thread_number)-1].pos.j = 2;
						}

						/* We exit the while */
						working_in_range = false;
					}
				} // while (working_in_range): apply the algorithm to the range
			} // if (we have action assigned)

			// If all the threads have finished, we notify the main thread.
			/* Critical section */
			access_lock.lock();
            {
			    object->im_threads_actions.threads_working--;
			    if (object->im_threads_actions.threads_working <= 0)
			    {
                    if (object->im_debug)
                    {
				        lock_cout.lock();
				        cout << "thread(" << thread_number << ")::wait_completion::notify" << endl;
				        lock_cout.unlock();
                    }
                    /*
                     * The barrier will block all the threads (including the main execution process)
                     * until the last thread calls the wait() member function of the barrier.
                     */
				    object->im_barrier_wait_completion->wait();
			    }
            }
			access_lock.unlock();
			/* Critical section */
		} // while (we_must_do_things)

		// The thread exits.
        if (object->im_debug)
        {
		    lock_cout.lock();
		    cout << "thread(" << thread_number << ")::end" << endl;
		    lock_cout.unlock();
        }
	}
	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////// pshrink_continue_condition
	// /////////////////////////////////////////////////////////////////////////////
	bool image::pshrink_continue_condition(void)
	{
        /*
         * Variables.
         */
        t_lock lock_cout(this->im_mutex_io, false);
        
		switch (this->im_threads_actions.algorithm)
		{
        case PARALLEL_SHRINK_ALGORITHM_NULL:
            if (this->im_debug)
            {
                lock_cout.lock();
                cerr << "[ERROR]::image::pshrink_continue_condition::NULL algorithm found" << endl;
                lock_cout.unlock();
            }
            break;

        case PARALLEL_SHRINK_ALGORITHM_FRONTIERS:
            return !(
						((this->im_threads_actions.stage % 4) == 0)
						&&
						!this->im_threads_actions.simple_points_found
                     );
            break;   
            
        case PARALLEL_SHRINK_ALGORITHM_RH:
            return (	!((this->im_threads_actions.stage % 4) == 0)
					||
						this->im_threads_actions.simple_points_found
					);
            break;
            
        case PARALLEL_SHRINK_ALGORITHM_ROS:
            return (	!((this->im_threads_actions.stage % 4) == 0)
					||
						this->im_threads_actions.simple_points_found
					);
            break;
       
        case  PARALLEL_SHRINK_ALGORITHM_GH89A1:
            return (	!((this->im_threads_actions.stage % 2) == 0)
					||
						this->im_threads_actions.simple_points_found
					);
            break;

        case PARALLEL_SHRINK_ALGORITHM_TSIN:
            return (	!((this->im_threads_actions.stage % 2) == 0)
					||
						this->im_threads_actions.simple_points_found
					);
            break;

        case  PARALLEL_SHRINK_ALGORITHM_GH89A2:
            return (	!((this->im_threads_actions.stage % 2) == 0)
					||
						this->im_threads_actions.simple_points_found
					);
            break;
		}
		/* This should not run */
		return false;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////// pshrink_algorithm_next_stage
	// /////////////////////////////////////////////////////////////////////////////
	void image::pshrink_algorithm_next_stage(void)
	{
		/*
		 * Variables.
		 */
		word stage;
        t_lock lock_cout(this->im_mutex_io, false);

		/*
		 * Next stage depends on the algorithm that is been applied.
		 */
		switch (this->im_threads_actions.algorithm)
		{
        case image::PARALLEL_SHRINK_ALGORITHM_NULL:
            if (this->im_debug)
            {
                lock_cout.lock();
                cerr << "pshrink_algorithm_next_stage::[ERROR]::NULL_algorithm_found" << endl;
                lock_cout.unlock();
            }
            break;

		case image::PARALLEL_SHRINK_ALGORITHM_FRONTIERS:
			stage = (this->im_threads_actions.stage + 1) % 4;
			if (stage == 0)
				stage = 4;
			break;		

		case image::PARALLEL_SHRINK_ALGORITHM_RH:
			stage = (this->im_threads_actions.stage + 1) % 4;
			if (stage == 0)
				stage = 4;
			break;
            
        case image::PARALLEL_SHRINK_ALGORITHM_ROS:
			stage = (this->im_threads_actions.stage + 1) % 4;
			if (stage == 0)
				stage = 4;
            break;

		case image::PARALLEL_SHRINK_ALGORITHM_GH89A1:
			stage = (this->im_threads_actions.stage + 1) % 2;
			if (stage == 0)
				stage = 4;
			break;

		case image::PARALLEL_SHRINK_ALGORITHM_TSIN:
			stage = (this->im_threads_actions.stage + 1) % 2;
			if (stage == 0)
				stage = 2;
            break;

		case image::PARALLEL_SHRINK_ALGORITHM_GH89A2:
			stage = (this->im_threads_actions.stage + 1) % 2;
			if (stage == 0)
				stage = 2;
			break;
		}
		/*
		 * Now we change the status.
		 */
		this->im_threads_actions.stage = stage;
        if (stage == 1)
			this->im_threads_actions.simple_points_found = false;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////////////////////////// read_byte
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: file es un flujo de file abierto en formato lectura.
	//
	// POST: Devuelve en result el byte de la posicion actual del puntero
	//       dentro del file.
	//
	// POST: result = file(pos_act)    Y
	//       file.pos_act = pos_act + 1
	//
	//       DONDE: pos_act = Posicion del puntero de lectura en 'file'
	//                        antes de la llamada.
	//
	image::byte image::read_byte (
		fstream& file)
	{
		// Lee un byte y lo devuelve en result (en este caso es trivial).
		char character;
		file.read (&character, 1);
		return (byte) character;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////////////////////////////// read_word
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: file es un flujo de file abierto en formato lectura.
	//
	// POST: Devuelve en 'result' la word_to_write formada por los dos bytes
	//       consecutivos de 'file', estando almacenados en formato little-endian.
	//
	// POST: result = file(pos_act) + ( file(pos_act + 1) * 2^9 )    Y
	//       file.pos_act = pos_act + 2
	//
	//       DONDE: pos_act = Posicion del puntero de lectura en 'file'
	//                        antes de la llamada.
	//
	image::word image::read_word (
		fstream& file)
	{
		// Lee dos bytes del file (en formato little-endian) y los devuelve
		// en el result.
		word result;

		result = (word) read_byte (file);
		result += ((word) read_byte (file)) << 8;

		return result;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////// read_dword
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: file es un flujo de file abierto en formato lectura.
	//
	// POST: Devuelve en 'result' la doble word_to_write formado por los cuatro bytes
	//       consecutivos de 'file', estando almacenados en formato little-endian.
	//
	// POST: result = file(pos_act) +
	//                   ( file(pos_act + 1) * 2^9) +
	//                   ( file(pos_act + 2) * 2^13 ) +
	//                   ( file(pos_act + 3) * 2^17 )    Y
	//       file.pos_act = pos_act + 4
	//
	//       DONDE: pos_act = Posicion del puntero de lectura en 'file'
	//                        antes de la llamada.
	//
	image::dword image::read_dword (
		fstream& file)
	{
		// Lee cuatro bytes del file (en formato little-endian) y los devuelve
		// en el result.
		dword result;

		result = (dword) read_word (file);
		result += ((dword) read_word (file)) << 16;

		return result;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////// extract_byte_from_dword
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: cierto
	//
	// POST: Devuelve los cuatro bits indicados (medio byte) dentro de la
	//       doble word_to_write dada.
	//
	//                             dword
	//                   word                   word
	//              byte         byte         byte         byte
	//          hbyte hbyte  hbyte hbyte  hbyte hbyte  hbyte hbyte
	//           1     2      3     4      5     6      7     8
	//          xxxx  xxxx - xxxx  xxxx - YYYY  xxxx - xxxx  xxxx
	//
	// POST: result = (double_word / 2^( 4*(8-half_word) )) & 0x000000F
	//
	image::dword image::extract_byte_from_dword (
		const dword double_word,
		const unsigned short int half_word)
	{
		// Variables que se usaran.
		dword result;

		// Doble word_to_write original
		// xxxx  xxxx - xxxx  xxxx - YYYY  xxxx - xxxx  xxxx
		// Desplazamiento
		// 0000  0000 - 0000  xxxx - xxxx  xxxx - xxxx  YYYY
		result = double_word >> (4*(8-half_word));

		// Ponemos a cero los bits por encima de 2^4
		// 0000  0000 - 0000  0000 - 0000  0000 - 0000  YYYY
		result = result & 0x0000000F;

		return result;
	}

	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////// write_dword_lendian
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: file es un flujo de file abierto en formato escritura.
	//
	// POST: Escribe en 'file' la doble word_to_write almacenandola en formato little-endian.
	//
	// POST: result = escribe_word (file, lword (double_word)) Y
	//                   escribe_word (file, hword (double_word))
	//
	//       DONDE: lword (double_word) = double_word & 0x0000FFFF
	//              hword (double_word) = (double_word / 2^(16)) & 0x0000FFFF
	//
	bool image::write_dword_lendian (
		fstream& file,
		const dword double_word)
	{
		return write_word_lendian (file, (word) (double_word & 0x0000FFFF))
			&& write_word_lendian (file, (word) ((double_word >> 16) & 0x0000FFFF));
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////// write_word_lendian
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: file es un flujo de file abierto en formato escritura.
	//
	// POST: Escribe en 'file' la word_to_write almacenandola en formato little-endian.
	//
	// POST: result = escribe_byte (file, lbyte (word_to_write)) Y
	//                   escribe_byte (file, hbyte (word_to_write))
	//
	//       DONDE: lbyte (word_to_write) = word_to_write & 0x00FF
	//              hbyte (word_to_write) = (word_to_write / 2^(8)-1) & 0x00FF
	//
	bool image::write_word_lendian (
		fstream& file,
		const word word_to_write)
	{
		return write_byte (file, (byte) (word_to_write & 0x00FF))
			&& write_byte (file, (byte) ((word_to_write >> 8) & 0x00FF));
	}

	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////////////////////// write_byte
	// /////////////////////////////////////////////////////////////////////////////
	//
	// PRE: file es un flujo de file abierto en formato escritura.
	//
	// POST: Escribe en 'file' el byte indicado.
	//
	// POST: result = file(pos_act) = byte_to_write Y
	//       file.pos_act = pos_act + 1
	//
	//       DONDE: pos_act = Posicion del puntero de lectura en 'file'
	//                        antes de la llamada.
	//
	bool image::write_byte (
		fstream& file,
		const byte byte_to_write)
	{
		// Variables que se usaran
		char character;

		character = (char) byte_to_write;
		file.write (&character, 1);
		return true;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////// is_simple_point
	// /////////////////////////////////////////////////////////////////////////////
	bool image::is_simple_point (
		const dword i,
		const dword j,
		const t_adjacence adjacence)
	{
		return is_simple_point (this->im_image_matrix, i, j, adjacence);
	}
	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////////////// is_simple_point
	// /////////////////////////////////////////////////////////////////////////////
	bool image::is_simple_point (
		t_colour **matrix,
		const dword i,
		const dword j,
		const t_adjacence adjacence)
	{
		if (adjacence == ADJ_8B_4W && 
			((
				(matrix[(i-1)-1][(j)-1] <= 0 &&

					(matrix[(i-1)-1][(j+1)-1] > 0 ||
					 matrix[(i)-1][(j+1)-1] > 0)) 

				+

				(matrix[(i)-1][(j+1)-1] <= 0 &&

					(matrix[(i+1)-1][(j+1)-1] > 0 ||
					 matrix[(i+1)-1][(j)-1] > 0))

				+

				(matrix[(i+1)-1][(j)-1] <= 0 &&

					(matrix[(i+1)-1][(j-1)-1] > 0 ||
					 matrix[(i)-1][(j-1)-1] > 0))

				+

				(matrix[(i)-1][(j-1)-1] <= 0 &&

					(matrix[(i-1)-1][(j-1)-1] > 0 ||
					 matrix[(i-1)-1][(j)-1] > 0))

				)==1? true: false)
			)
		{
			// Is simple point
			return true;
		}
		else if (adjacence == ADJ_4B_8W &&
				((
					(matrix[(i-1)-1][(j)-1] > 0 &&

						(matrix[(i-1)-1][(j+1)-1] <= 0 ||
						 matrix[(i)-1][(j+1)-1] <= 0))

					+

					(matrix[(i)-1][(j+1)-1] > 0 &&

						(matrix[(i+1)-1][(j+1)-1] <= 0 ||
						 matrix[(i+1)-1][(j)-1] <= 0))

					+

					(matrix[(i+1)-1][(j)-1] > 0 &&

						(matrix[(i+1)-1][(j-1)-1] <= 0 ||
						 matrix[(i)-1][(j-1)-1] <= 0))

					+

					(matrix[(i)-1][(j-1)-1] > 0 &&

						(matrix[(i-1)-1][(j-1)-1] <= 0 ||
						 matrix[(i-1)-1][(j)-1] <= 0))

				)==1? true: false)
			)
		{
			// Is simple point
			return true;
		}
		// Is not simple point.
		return false;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////////// meets_shrink_rh_conditions
	// /////////////////////////////////////////////////////////////////////////////
	bool image::meets_shrink_rh_conditions (
		const dword i,
		const dword j,
		t_frontier frontier)
	{
		// Variable
		bool result;

		// Condition 1
		result = is_simple_point (this->im_pshrink_initial_image_matrix,
								  i,
								  j,
								  image::ADJ_8B_4W);

		// Condition 2
		result = result &&
				 (is_frontier (this->im_pshrink_initial_image_matrix,
							   i,
							   j,
							   frontier)
				  ||
				  black_adjacent_points (this->im_pshrink_initial_image_matrix,
										 i,
										 j,
										 image::ADJ_8B_4W) == 1
				  );

		// Condition 3 (left to right in documentation).
		result = result && !(
			                 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i, j, image::ADJ_8B_4W) == 1
							 &&
							 i > 1 && j > 1
							 &&
							 this->im_pshrink_initial_image_matrix[(i-1)-1][(j-1)-1] > 0
							 &&
							 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i-1, j-1, image::ADJ_8B_4W) == 1
							 );

		result = result && !(
			                 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i, j, image::ADJ_8B_4W) == 1
							 &&
							 i > 1 && j < this->im_width
							 &&
							 this->im_pshrink_initial_image_matrix[(i-1)-1][(j+1)-1] > 0
							 &&
							 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i-1, j+1, image::ADJ_8B_4W) == 1
							 );

		result = result && !(
			                 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i, j, image::ADJ_8B_4W) == 1
							 &&
							 i > 1
							 &&
							 this->im_pshrink_initial_image_matrix[(i-1)-1][(j)-1] > 0
							 &&
							 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i-1, j, image::ADJ_8B_4W) == 1
							 );

		result = result && !(
			                 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i, j, image::ADJ_8B_4W) == 1
							 &&
							 j > 1
							 &&
							 this->im_pshrink_initial_image_matrix[(i)-1][(j-1)-1] > 0
							 &&
							 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i, j-1, image::ADJ_8B_4W) == 1
							 );

		return result;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////// meets_shrink_ros_conditions
	// /////////////////////////////////////////////////////////////////////////////
	bool image::meets_shrink_ros_conditions (
		const dword i,
		const dword j,
		t_frontier frontier)
	{
		// Variable
		bool result;

		// Condition 1
		result = is_simple_point (this->im_pshrink_initial_image_matrix,
								  i,
								  j,
								  image::ADJ_8B_4W);

		// Condition 2
		result = result &&
				 (is_frontier (this->im_pshrink_initial_image_matrix,
							   i,
							   j,
							   frontier)
				  &&
				  black_adjacent_points (this->im_pshrink_initial_image_matrix,
										 i,
										 j,
										 image::ADJ_8B_4W) > 1
				  );

		// Condition 3 (left to right in documentation).
		result = result && !(
			                 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i, j, image::ADJ_8B_4W) == 1
							 &&
							 i > 1 && j > 1
							 &&
							 this->im_pshrink_initial_image_matrix[(i-1)-1][(j-1)-1] > 0
							 &&
							 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i-1, j-1, image::ADJ_8B_4W) == 1
							 );

		result = result && !(
			                 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i, j, image::ADJ_8B_4W) == 1
							 &&
							 i > 1 && j < this->im_width
							 &&
							 this->im_pshrink_initial_image_matrix[(i-1)-1][(j+1)-1] > 0
							 &&
							 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i-1, j+1, image::ADJ_8B_4W) == 1
							 );

		result = result && !(
			                 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i, j, image::ADJ_8B_4W) == 1
							 &&
							 i > 1
							 &&
							 this->im_pshrink_initial_image_matrix[(i-1)-1][(j)-1] > 0
							 &&
							 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i-1, j, image::ADJ_8B_4W) == 1
							 );

		result = result && !(
			                 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i, j, image::ADJ_8B_4W) == 1
							 &&
							 j > 1
							 &&
							 this->im_pshrink_initial_image_matrix[(i)-1][(j-1)-1] > 0
							 &&
							 black_adjacent_points (this->im_pshrink_initial_image_matrix,
													i, j-1, image::ADJ_8B_4W) == 1
							 );

		return result;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////// meets_shrink_gh89a1_conditions
	// /////////////////////////////////////////////////////////////////////////////
	bool image::meets_shrink_gh89a1_conditions (
		const dword i,
		const dword j,
		const byte num_iteration)
	{
		// Variable
		bool result;
		dword adjacent_points;
		image::t_points_set *set_adjacents;

		// Condition 1
		result = is_simple_point (i, j, image::ADJ_8B_4W);

		// Condition 2
		set_adjacents = points_set_create_black_adjacents (
								this->im_pshrink_initial_image_matrix,
								i,
								j,
								image::ADJ_8B_4W);

		result = result && image::points_set_is_connected (
								this->im_pshrink_initial_image_matrix,
								set_adjacents,
								image::ADJ_8B_4W);

		// Condition 3
		adjacent_points = image::black_adjacent_points (
								this->im_pshrink_initial_image_matrix,
								i,
								j,
								image::ADJ_8B_4W);

		result = result && !(
							 (adjacent_points == 1 || adjacent_points == 2)
							 &&
							 image::points_set_is_connected (set_adjacents, image::ADJ_4B_8W) 
							 );

		// Condition 4
		if (result && (num_iteration % 2) == 0)
		{
			// Even iterations
			if (	 j < this->im_width &&
						this->im_pshrink_initial_image_matrix[(i)-1][(j+1)-1] <= 0
				||
					(i > 1 &&
						this->im_pshrink_initial_image_matrix[(i-1)-1][(j)-1] <= 0 &&
					 i > 1 && j < this->im_width &&	
						this->im_pshrink_initial_image_matrix[(i-1)-1][(j+1)-1] <= 0 &&	
					 i < this->im_height && j < this->im_width &&
						this->im_pshrink_initial_image_matrix[(i+1)-1][(j+1)-1] > 0)
				)
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}
		else if (result && (num_iteration % 2) == 1)
		{
			// Odd iterations
			if (	 j > 1 &&
						this->im_pshrink_initial_image_matrix[(i)-1][(j-1)-1] <= 0
				||
					(i > 1 && j > 1 &&
						this->im_pshrink_initial_image_matrix[(i-1)-1][(j-1)-1] > 0 &&
					 i > 1 && j < this->im_width &&
						this->im_pshrink_initial_image_matrix[(i-1)-1][(j+1)-1] <= 0 &&	
					 i < this->im_height &&
						this->im_pshrink_initial_image_matrix[(i+1)-1][(j)-1] <= 0)
				)
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}

		return result;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// /////////////////////////////////////////////// meets_shrink_tsin_conditions
	// /////////////////////////////////////////////////////////////////////////////
	bool image::meets_shrink_tsin_conditions (
		const dword i,
		const dword j,
		const byte num_iteration)
	{
		// Variable
		bool result;
		dword adjacent_points;
		image::t_points_set *set_adjacents;

		// Condition 1
		result = is_simple_point (this->im_pshrink_initial_image_matrix, i, j, image::ADJ_8B_4W);

		// Condition 2
		set_adjacents = points_set_create_black_adjacents (
								this->im_pshrink_initial_image_matrix,i, j, image::ADJ_8B_4W);
		result = result && image::points_set_is_connected (
										set_adjacents, image::ADJ_8B_4W);

		// Condition 3
		adjacent_points = image::black_adjacent_points (
										this->im_pshrink_initial_image_matrix, i, j, image::ADJ_8B_4W);
		result = result && !(
							 (adjacent_points == 1 || adjacent_points == 2)
							 &&
							 image::points_set_is_connected (
										this->im_pshrink_initial_image_matrix, set_adjacents, image::ADJ_4B_8W) 
							 );

		// Condition 4
		if (result && (num_iteration % 2) == 0)
		{
			// Even iterations
			if (
					!(i > 1 && j > 1 &&
						this->im_pshrink_initial_image_matrix[(i-1)-1][(j-1)-1] <= 0 &&
					  i > 1 &&
						this->im_pshrink_initial_image_matrix[(i-1)-1][(j)-1] > 0 &&
					  j > 1 &&
						this->im_pshrink_initial_image_matrix[(i)-1][(j-1)-1] > 0 &&
					  j < this->im_width &&
						this->im_pshrink_initial_image_matrix[(i)-1][(j+1)-1] <= 0)
				&&
					!(i > 1 &&
						this->im_pshrink_initial_image_matrix[(i-1)-1][(j)-1] <= 0 &&
					  j < this->im_width &&
						this->im_pshrink_initial_image_matrix[(i)-1][(j+1)-1] > 0 &&
					  i < this->im_height && j < this->im_width &&
						this->im_pshrink_initial_image_matrix[(i+1)-1][(j+1)-1] <= 0 &&
					  i < this->im_height &&
						this->im_pshrink_initial_image_matrix[(i+1)-1][(j)-1] > 0)
				)
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}
		else if (result && (num_iteration % 2) == 1)
		{
			// Odd iterations
			if (
					!(j > 1 &&
						this->im_pshrink_initial_image_matrix[(i)-1][(j-1)-1] <= 0 &&
					  j < this->im_width &&
						this->im_pshrink_initial_image_matrix[(i)-1][(j+1)-1] > 0 &&
					  i < this->im_height &&
						this->im_pshrink_initial_image_matrix[(i+1)-1][(j)-1] > 0 &&
					  i < this->im_height && j < this->im_width &&
						this->im_pshrink_initial_image_matrix[(i+1)-1][(j+1)-1] <= 0)
				&&
					!(i > 1 && j > 1 &&
						this->im_pshrink_initial_image_matrix[(i-1)-1][(j-1)-1] <= 0 &&
					  i > 1 &&
						this->im_pshrink_initial_image_matrix[(i-1)-1][(j)-1] > 0 &&
					  j > 1 &&
						this->im_pshrink_initial_image_matrix[(i)-1][(j-1)-1] > 0 &&
					  i < this->im_height &&
						this->im_pshrink_initial_image_matrix[(i+1)-1][(j)-1] >= 0)
				)
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}

		return result;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// ///////////////////////////////////////////// meets_shrink_gh89a2_conditions
	// /////////////////////////////////////////////////////////////////////////////
	bool image::meets_shrink_gh89a2_conditions (
		const dword i,
		const dword j)
	{
		// Variables
		bool result;

		// The point must fit the conditions
		// Condition 1
		result = image::is_simple_point (this->im_pshrink_initial_image_matrix, i, j, image::ADJ_8B_4W);

		// Condition 2
		result = result &&
				 (image::black_adjacent_points (this->im_pshrink_initial_image_matrix, i, j, image::ADJ_8B_4W) >= 2);

		// Final result
		return result;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////// points_set_is_connected
	// /////////////////////////////////////////////////////////////////////////////
	bool image::points_set_is_connected (
		t_points_set * points_set,
		const t_adjacence adjacence)
	{
		return points_set_is_connected (this->im_image_matrix, points_set, adjacence);
	}
	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////// points_set_is_connected
	// /////////////////////////////////////////////////////////////////////////////
	bool image::points_set_is_connected (
		t_colour **matrix,
		t_points_set * points_set,
		const t_adjacence adjacence)
	{
		// Variables
		t_points_set::iterator iterator1;
		t_points_set::iterator iterator2;
		byte num_adjacents;
		dword i;
		dword j;
		t_point adjacent_point;
		bool any_adjacent_is_in_the_set;

		// Each point must have, at least, one adjacent point from the set.
		// If all the points meets this condition, the set is connected.
		for (iterator1 = points_set->begin();
			 iterator1 != points_set->end();
			 iterator1++)
		{
			// The point must fit the condition.
			i = (*iterator1).i;
			j = (*iterator1).j;
			num_adjacents = black_adjacent_points (matrix, i, j, adjacence);

			// If it is NOT the only point in the set, but it has
			// no adjacent points, then the set of points is not connected.
			if (num_adjacents == 0 && points_set->size () > 1)
			{
				return false;
			}

			any_adjacent_is_in_the_set = false;
			// Adjacent points:
			//   1 2 3            2
			//   4 x 5          4 x 5
			//   6 7 8            7
			// Is adjacent 1 in the set?
			if (adjacence == ADJ_8B_4W &&
				i > 1 &&
				j > 1 &&
				matrix[(i-1)-1][(j-1)-1] > 0)
			{
				// There is an adjacent point in possition 1:
				// now we search for this adjacent in the set.
				adjacent_point.i = i-1;
				adjacent_point.j = j-1;
				iterator2 = points_set->find (adjacent_point);
				if (iterator2 != points_set->end())
				{
					// This adjacent is in the set
					any_adjacent_is_in_the_set = true;
				}
			}
			// Is adjacent 2 in the set?
			if (i > 1 &&
				matrix[(i-1)-1][(j)-1] > 0)
			{
				// There is an adjacent point in possition 1:
				// now we search for this adjacent in the set.
				adjacent_point.i = i-1;
				adjacent_point.j = j;
				iterator2 = points_set->find (adjacent_point);
				if (iterator2 != points_set->end())
				{
					// This adjacent is in the set
					any_adjacent_is_in_the_set = true;
				}
			}
			// Is adjacent 3 in the set?
			if (adjacence == ADJ_8B_4W &&
				i > 1 &&
				j < this->im_width &&
				matrix[(i-1)-1][(j+1)-1] > 0)
			{
				// There is an adjacent point in possition 1:
				// now we search for this adjacent in the set.
				adjacent_point.i = i-1;
				adjacent_point.j = j+1;
				iterator2 = points_set->find (adjacent_point);
				if (iterator2 != points_set->end())
				{
					// This adjacent is in the set
					any_adjacent_is_in_the_set = true;
				}
			}
			// Is adjacent 4 in the set?
			if (j > 1 &&
				matrix[(i)-1][(j-1)-1] > 0)
			{
				// There is an adjacent point in possition 1:
				// now we search for this adjacent in the set.
				adjacent_point.i = i;
				adjacent_point.j = j-1;
				iterator2 = points_set->find (adjacent_point);
				if (iterator2 != points_set->end())
				{
					// This adjacent is in the set
					any_adjacent_is_in_the_set = true;
				}
			}
			// Is adjacent 5 in the set?
			if (j < this->im_width &&
				matrix[(i)-1][(j+1)-1] > 0)
			{
				// There is an adjacent point in possition 1:
				// now we search for this adjacent in the set.
				adjacent_point.i = i;
				adjacent_point.j = j+1;
				iterator2 = points_set->find (adjacent_point);
				if (iterator2 != points_set->end())
				{
					// This adjacent is in the set
					any_adjacent_is_in_the_set = true;
				}
			}
			// Is adjacent 6 in the set?
			if (adjacence == ADJ_8B_4W &&
				i < this->im_height &&
				j > 1 &&
				matrix[(i+1)-1][(j-1)-1] > 0)
			{
				// There is an adjacent point in possition 1:
				// now we search for this adjacent in the set.
				adjacent_point.i = i+1;
				adjacent_point.j = j-1;
				iterator2 = points_set->find (adjacent_point);
				if (iterator2 != points_set->end())
				{
					// This adjacent is in the set
					any_adjacent_is_in_the_set = true;
				}
			}
			// Is adjacent 7 in the set?
			if (i < this->im_height &&
				matrix[(i+1)-1][(j)-1] > 0)
			{
				// There is an adjacent point in possition 1:
				// now we search for this adjacent in the set.
				adjacent_point.i = i+1;
				adjacent_point.j = j;
				iterator2 = points_set->find (adjacent_point);
				if (iterator2 != points_set->end())
				{
					// This adjacent is in the set
					any_adjacent_is_in_the_set = true;
				}
			}
			// Is adjacent 8 in the set?
			if (adjacence == ADJ_8B_4W &&
				i < this->im_height &&
				j < this->im_width &&
				matrix[(i+1)-1][(j+1)-1] > 0)
			{
				// There is an adjacent point in possition 1:
				// now we search for this adjacent in the set.
				adjacent_point.i = i+1;
				adjacent_point.j = j+1;
				iterator2 = points_set->find (adjacent_point);
				if (iterator2 != points_set->end())
				{
					// This adjacent is in the set
					any_adjacent_is_in_the_set = true;
				}
			}
			if (num_adjacents > 0 &&
				! any_adjacent_is_in_the_set)
			{
				// This point has adjacent points but none of
				// them is in the set.
				return false;
			}

			// This point is x-connected to one or more
			// points in the set, so the set of points is
			// x-connected by now.

		} // for (points in the set)
		
		// All the points in the set fit the condition, so the set
		// is x-connected.
		return true;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// //////////////////////////////////////////////////// points_set_intersection
	// /////////////////////////////////////////////////////////////////////////////
	image::t_points_set * image::points_set_intersection (
		const t_points_set &set1,
		const t_points_set &set2)
	{
		// Variables
		t_points_set *intersection;
		t_points_set::iterator iterator1;
		t_points_set::iterator iterator2;
		t_point point_set1;

		// First we create the set.
		intersection = new t_points_set();

		// Now we do the intersection of two sets.
		for (iterator1 == set1.begin();
			 iterator1 != set1.end();
			 iterator1++)
		{
			// We extract the point from set 1.
			point_set1.i = (*iterator1).i;
			point_set1.j = (*iterator2).j;

			// We search for that point in set 2.
			if (set2.find(point_set1) != set2.end())
			{
				// We add the point to the intersection
				intersection->insert (point_set1);
			}
		} // for (point in set 1)
		
		return intersection;
	}
	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////// points_set_create_black_adjacents
	// /////////////////////////////////////////////////////////////////////////////
	image::t_points_set * image::points_set_create_black_adjacents (
		const dword i,
		const dword j,
		const t_adjacence adjacence)
	{
		return points_set_create_black_adjacents (this->im_image_matrix, i, j, adjacence);
	}
	// /////////////////////////////////////////////////////////////////////////////
	// ////////////////////////////////////////// points_set_create_black_adjacents
	// /////////////////////////////////////////////////////////////////////////////
	image::t_points_set * image::points_set_create_black_adjacents (
		t_colour ** matrix,
		const dword i,
		const dword j,
		const t_adjacence adjacence)
	{
		// Variables.
		t_points_set *points_set;
		t_point point;

		// We create the set.
		points_set = new t_points_set();

		// We add the points to the set
		// 1 2 3
		// 4 x 5
		// 6 7 8

		// Add the adjacent point 1
		if (adjacence == image::ADJ_8B_4W &&
			i > 1 &&
			j > 1 &&
			matrix[(i-1)-1][(j-1)-1] > 0)
		{
			point.i = i-1;
			point.j = j-1;
			points_set->insert (point);
		}
		// Add the adjacent point 2
		if (i > 1 &&
			matrix[(i-1)-1][(j)-1] > 0)
		{
			point.i = i-1;
			point.j = j;
			points_set->insert (point);
		}
		// Add the adjacent point 3
		if (adjacence == image::ADJ_8B_4W &&
			i > 1 &&
			j < this->im_width &&
			matrix[(i-1)-1][(j+1)-1] > 0)
		{
			point.i = i-1;
			point.j = j+1;
			points_set->insert (point);
		}
		// Add the adjacent point 4
		if (j > 1 &&
			matrix[(i)-1][(j-1)-1] > 0)
		{
			point.i = i;
			point.j = j-1;
			points_set->insert (point);
		}
		// Add the adjacent point 5
		if (j < this->im_width &&
			matrix[(i)-1][(j+1)-1] > 0)
		{
			point.i = i;
			point.j = j+1;
			points_set->insert (point);
		}
		// Add the adjacent point 6
		if (adjacence == image::ADJ_8B_4W &&
			i < this->im_height &&
			j > 1 &&
			matrix[(i+1)-1][(j-1)-1] > 0)
		{
			point.i = i+1;
			point.j = j-1;
			points_set->insert (point);
		}
		// Add the adjacent point 7
		if (i < this->im_height &&
			matrix[(i+1)-1][(j)-1] > 0)
		{
			point.i = i+1;
			point.j = j;
			points_set->insert (point);
		}
		// Add the adjacent point 8
		if (adjacence == image::ADJ_8B_4W &&
			i < this->im_height &&
			j < this->im_width &&
			matrix[(i+1)-1][(j+1)-1] > 0)
		{
			point.i = i+1;
			point.j = j+1;
			points_set->insert (point);
		}

		// We have created the set.
		return points_set;
	}
}
