/**
 *
 * \file operations.h
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

#ifndef TOPDIG_OPERATIONS_HPP
#define TOPDIG_OPERATIONS_HPP

#include <iostream>
using namespace std;

#include "image.h"
using namespace topdig;

/**
 * \class operations
 * This class apply topological algorithms to a digital image.
 *
 * This class uses the image class to apply some topological
 * algorithms to a bitmap image.
 * The algorithms to apply are read from a operations text file
 * wich includes one operation (algorithm) and its arguments
 * in its lines (one operation per line).
 * Here is a sample operations text file that we could apply
 * to a digital image:
 *
 * \par operations_file.txt
 * white_frame 1 \n
 * medial_axis_transformation 8_4 \n
 * ma 8_4 \n
 * borders 8_4 \n
 * black_components 8_4 \n
 * sequentiall_shrink borders 8_4 \n
 *
 * The previous file will apply the following algorithms to a digital
 * image: first it will create a white frame of 1 points around the original
 * image, then it will calculate de MAT (Medial Axis Transformation) of the
 * result image, then MA (points of MAT wich are locale maximum), then it
 * will calculate the borders of the black components (components of black
 * points), then it will label each black component with a different positive
 * number, and after all, it will thin the image using by removing the simple
 * points of the black components that are in the borders.
 *
 * \note Some algorithms have one argument, others two... But the argument
 * adjacence is common to most of them.
 */

class operations
{
public:

	/**
	 * Type that defines the number of output files.
	 *
	 * Defines the way results are stored on bitmap images:
	 *    - OUTPUT_DIFF: generates one bitmap per operation applyed
	 *           to the original image.
	 *    - OUTPUT_RESULT: generates one bitmap with the result of
	 *           applying all the operations to the original image.
	 */
	enum t_screen_output
	{
		OUTPUT_DIFF = 1,
		OUTPUT_RESULT = 10
	};
	/**
	 * Type that represents the topological operations that can be
	 * applyed to the image.
	 *
	 * This type should cointain all the posible algorithms that
	 * the class image can apply.
	 */
	enum t_operation
	{
		OPERATION_UNKNOWN = 0,
		OPERATION_WHITE_FRAME = 1,
		OPERATION_BLACK_FRAME = 2,
		OPERATION_BLACK_COMPONENTS = 4,
		OPERATION_WHITE_COMPONENTS = 8,
		OPERATION_EDGES = 16,
		OPERATION_MAT = 32,
		OPERATION_MA = 64,
		OPERATION_REBUILD_FROM_MA = 128,
		OPERATION_SIMPLE_POINTS_FIND = 256,
		OPERATION_SIMPLE_POINTS_REMOVE = 512,
		OPERATION_SEQUENTIAL_SHRINK = 1024,
		OPERATION_PARALLEL_SHRINK = 2048
	};
	/**
	 * Class constructor.
	 *
	 * \pre
	 *      Actually exists image_file and operations_file, and their format
	 *      is correct acording to the specification. Otherwise, read could fail.
	 *
	 * \post
	 *      Creates an image object from the bitmap file provided, and apply to
	 *      it the operations contained in the operations file, one a time in
	 *      order of appearance in the file (one operation per line).
	 *      The output is generated in two different ways:
	 *         - if output_format = OUTPUT_DIFF: it generates one file after
	 *              applyind each of the operations. The filename will be
	 *              the original ones, with _xxx appended before the extension.
	 *              For example: if we apply three operations to the image
	 *              "my_image.bmp", the result will be three bitmaps named
	 *              "my_image_001.bmp", "my_image_002.bmp" and "my_image_003.bmp".
	 *         - if output_format = OUTPUT_RESULT: it generates one file after
	 *              applying all the operations to the original image. The filename
	 *              will be the same, with "_FINAL" appended before the extension.
	 *              IE: "my_image.bmp" -> "my_image_FINAL.bmp"
	 *
	 * \param[in] image_file
	 *        Full path of the bitmap file from wich image data will be read.
	 *        It must be in the format of the specification for the "load" member
	 *        of the image class.
	 *
	 * \param[in] operations_file
	 *        Full path of the operations text file, that contains the topological
	 *        algorithms to be applyed to the image. There must be one operation
	 *        per line, including the possible arguments.
	 *
	 * \param[in] output_format
	 *        Determines the number of output files.
	 *
	 * \return
	 *         The objet operation with all the algorithms contained in the
	 *         operations file to the original image. The result images will be in
	 *         the same folder than the original one.
	 *
	 * \sa t_operation
	 */
	operations(
		string image_file,
		string operations_file,
        bool debug = false,
		t_screen_output number_output_files = OUTPUT_DIFF);
	/**
	 * Class destructor.
	 *
	 * Destroy the image object deallocating the memory used, close the files, and
	 * frees the memory used by this object.
	 */
	~operations(void);

protected:
	/**
	 * Represents the kind of line readed from the file.
	 */
	enum t_line
	{
		/**
		 * Represents a blank line in the file.
		 */
		LINE_BLANK,
		/**
		 * Represents a comment line in the file (lines stating with '#')
		 */
		LINE_COMMENT,
		/**
		 * First character in line is invalid.
		 */
		LINE_SINTAX_ERROR,
		/**
		 * First character in line is valid.
		 */
		LINE_OPERATION
	};
	/**
	 * Buffer type for the lines of the operation file.
	 */
	struct t_buffer
	{
		/**
		 * Buffer contents.
		 *
		 * It is limited to 1024 because there is not an operation wich
		 * length (including the arguments and the spaces) is longer.
		 *
		 * \warning
		 * When adding new operations to the class image, and then to this class,
		 * if one exceeds the maximum length, it must be modified to avoid a
		 * buffer overflow.
		 */
		char content[1024];
		/**
		 * Read pointer position in the buffer.
		 */
		char *pos;
	};
	/**
	 * Type for storing operations arguments.
	 *
	 * This type stores all the possible arguments of the operations.
	 * Each operation has its own arguments, that will be stored with this
	 * general type.
	 *
	 * \note
	 * When adding new operations to the class image, and then to this class,
	 * it will be necessay to add more members to this structure if the
	 * arguments of the new operations are not the same as any of the actual.
	 */
	struct t_argumentos
	{
		/**
		 * Width of the frame.
		 *
		 * This argument is used in: OPERATION_WHITE_FRAME, OPERATION_BLACK_FRAME
		 */
		image::dword frame_width;
		/**
		 * Adjacence of the image: (8,4)-image or (4,8)-image.
		 */
		image::t_adjacence adjacence;
		/**
		 * Algorithm used in image shrink.
		 *
		 * This argument is used in: OPERATION_SIMPLE_POINTS_FIND, OPERATION_SIMPLE_POINTS_REMOVE,
		 * OPERATION_SEQUENTIAL_SHRINK
		 */
		image::t_simple_points_algorithm sp_algorithm;
		/**
		 *  Algorithm used in parallel image shrink
		 *
		 * This argument is used in: OPERATION_PARALLEL_SHRINK
		 */
		image::t_parallel_shrink_algorithm parallel_algorithm;
	};
	/**
	 * Tests the first character of the line extracted from the operations file.
	 *
	 * \pre
	 *      True.
	 *
	 * \post
	 *      All the lines of the operations file starts with the mnemonic
	 *      assigned to the operation, so the first character in each line
	 *      must be a capital letter. A comment is a line that starts with
	 *      a different character.
	 *
	 * \param[in] buffer
	 *        Pointer to the first character in the line buffer readed from
	 *        the operations file.
	 *
	 * \return
	 *         'true' if the first character of the buffer is a capital letter,
	 *         or 'false' in any other case.
	 */
	t_line check_line (
		char *buffer);
	/**
	 * Extracts the operation code from the buffer.
	 *
	 * \pre
	 *      True.
	 *
	 * \post
	 *      Extracts the operation code acording to the mnemonic stored in the
	 *      first characters of the buffer (until the first blank space).
	 *
	 * \param[in,out] buffer
	 *        Buffer that contains the line of text extracted from the operations
	 *        file. This member will read all characters until blanck space, end
	 *        of line (EOL) or end of file (EOF), and will try to determine the
	 *        code corresponding to the string.
	 *        The pointer will be at the first character of the arguments (if it exists).
	 *
	 * \param[in,out] operation
	 *        This member stores in this in-out parameter the code of the operation that
	 *        corresponds to the mnemonic found in the buffer.
	 *
	 * \return
	 *         'true' y and the code of the operation in 'operation' if all goes good
	 *         or 'false' in any other case (incorrect sintaxis or unknown operation).
	 */
	bool extract_operation (
		t_buffer &buffer,
		t_operation &operation);
	/**
	 * Extracts the arguments from the buffer.
	 *
	 * \pre
	 *      The buffer's pointer is in the first letter of the first argument, in case
	 *      the operation needs arguments.
	 *
	 * \post
	 *      Extracts the arguments from the buffer acording to the operation code
	 *      provided as argument, because the number and type of them will change from
	 *      one operation to another.
	 *      This member will store the arguments in the in-out parameter 'arguments'.
	 *      The 'arguments' struct members that are not necessary are not touched.
	 *
	 * \param[in,out] buffer
	 *        Buffer that contains the line of text extracted from the operations file.
	 *
	 * \param[in] operation
	 *        The code of the operation for wich the arguments are asked to be extracted.
	 *
	 * \param[in,out] arguments
	 *        This member will store in this in-out param the arguments read from the buffer.
	 *
	 * \return
	 *         'true' and the arguments (if the operation has arguments) if all is ok,
	 *         or 'false' in any other case (incorrect sintaxis or arguments not recognized).
	 */
	bool extract_arguments (
		t_buffer &buffer,
		t_operation operation,
		t_argumentos &arguments);
};

#endif
