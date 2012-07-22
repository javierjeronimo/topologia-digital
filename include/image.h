/**
 *
 * \file image.h
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

/** We dont want to include this file more than one */
#ifndef TOPDIG_IMAGE_HPP
#define TOPDIG_IMAGE_HPP

/** To support dynamic linking in a non Windows compiling environment */
#ifndef WIN32
#define BOOST_ALL_DYN_LINK 1
#endif
 
/** To support autolink in a Windows compiling environment */
#ifndef BOOST_ALL_DYN_LINK
#if !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_WHATEVER_NO_LIB) && !defined(BOOST_WHATEVER_SOURCE)
#define BOOST_LIB_NAME boost_thread
#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_WHATEVER_DYN_LINK)
#define BOOST_DYN_LINK
#endif
#include "boost/config/auto_link.hpp"
#endif
#endif

/** Things we need */
#include <iostream>
#include <set>

/** Multiplatform concurrent programming */
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/barrier.hpp>

#define COLOR_BLACK 0
#define COLOR_WHITE 1

namespace topdig
{
	/**
	 * \class image
	 * Represents an image with a matrix of points.
	 *
	 * \par
	 * Stores an image an permits to apply some topological algorithms on it.
	 * The image can be read from a bitmap (BMP format) with some restrictions:
	 * no compression (RGB compression in BMP documentation), 24bpp o 32bpp (even if
	 * the image is in black and white - 1 bpp - it must be in 24bpp or 32bpp:
	 * 0xFFFFFF, 0x000000).
	 *
	 * \par
	 * The result of applying the operations is that each point of the image will
	 * have a different label. The label of the points will provide topological
	 * information about the image. Black points (coloured points) will be labelled
	 * with positive (non zero) integers, and white points (non coloured points)
	 * with zero or negative integers.
	 *
	 * \par
	 * Initially, the image has black points labelled with '1' and white points
	 * labelled with '0', and, for example, after applying 'black_components'
	 * algorithm (which label all the points of a component with the same label
	 * and each black component with a different label for its points), the
	 * white points will still have its previous label, and the black points
	 * will be labelled with '1', '2', ... 'n', being 'n' the number of black
	 * components of the image.
	 */
	class image
	{

	public:

		/**
		 * Represents a byte or half word.
		 */
		typedef unsigned char byte;
		/**
		 * Represents a word.
		 */
		typedef unsigned short int word;
		/**
		 * Represents a double word.
		 */
		typedef unsigned int dword;
		/**
		 * Represents the colour of a point in the image:
         *    - >0 : black point.
         *    - <=0 : white point.
         */
		typedef short int t_colour;
		/**
		 * Represents the compression format of the source bitmap file.
		 *
		 * \warning The actual implementation of the library only supports BI_RGB (no compression).
		 */
		enum t_compression
		{
			/**
			 * No compression.
			 */
			BI_RGB,
			/**
			 * TODO
			 */
			BI_RLE8,
			/**
			 * TODO
			 */
			BI_RLE4,
			/**
			 * TODO
			 */
			BI_BITFIELDS,
			/**
			 * TODO
			 */
			BI_JPEG,
			/**
			 * TODO
			 */
			BI_PNG
		};
		/**
		 * Represents the way colours of the source image are translated
		 * into black or white.
		 * Each point of the image has a group of three components that identify
		 * its colour (traditional RGB), but in topological algorithms we only need to know
		 * whether a point is black or white (colour vs. no colour), so we must
		 * translate each real colour into this two posibilities.
		 */
		enum t_load_format
		{
			/**
			 * We only search for black (0xFFFFFF) and white (0x000000).
			 */
			LOAD_BW,
			/**
			 * We only look at the red component of the colour. Again: black 0xFF, and white 0x00.
			 */
			LOAD_RED,
			/**
			 * We only look at the green component of the colour. Again: black 0xFF, and white 0x00.
			 */
			LOAD_GREEN,
			/**
			 * We only look at the blue component of the colour. Again: black 0xFF, and white 0x00.
			 */
			LOAD_BLUE,
			/**
			 * We take the average of the three components of the colour: (red + blue + green) / 3,
			 * and if the result is less or equal than 127 it is translated into 0xFF (black), or
			 * in 0x00 (white) if it is greater than 127.
			 */
			LOAD_AVERAGE
		};
		/**
		 * Represents the way each point of the image will be stored in a bitmap file.
		 * It is related with the translation of the label of a point to a colour.
		 *
		 * \note When we apply a topological algorithm to the image, we usually label each point
		 * to see the result (ie, different labels on the border of a component, on different
		 * components...).
		 */
		enum t_save_format
		{
			/**
			 * It stores the image as a black and white bitmap.
			 */
			SAVE_BW,
			/**
			 * Is stores the image in gray, with the borders in black.
			 */
			SAVE_BORDES,
			/**
			 * It stores the image with a different colour for each component.
			 */
			SAVE_COMPONENTS,
			/**
			 * It stores the image with the same colour for the different values
			 * of the MAT.
			 */
			SAVE_MAT,
			/**
			 * It stores the image in gray, with the points of MA in black.
			 */
			SAVE_MA,
			/**
			 * It stores the image in black, with the simple points found in gray.
			 */
			SAVE_SIMPLE_POINTS_FIND,
			/**
			 * It stores the image in black, with the simple points removed in gray.
			 */
			SAVE_SIMPLE_POINTS_REMOVE
		};
		/**
		 * Represents the adjacence that will be used in the algorithms.
		 * It does not need to be the same in all the operations applied to the
		 * image, so this type allows to change the adjacence for each
		 * operation applied (pe, to see the differences).
		 *
		 * \note Default for most algorithms is (8,4)-image: ADJ_8B_4W.
		 */
		enum t_adjacence
		{
			/**
			 * The adjacence used will be (8,4), which stands for a
             * (8,4)-image (a black point has 8 adjacent points).
			 */
			ADJ_4B_8W = true,
			/**
			 * The adjacence used will be (4,8) (a black point has 4 adjacent
             * points).
			 */
			ADJ_8B_4W = false
		};
		/**
		 * Represents the algorithm used when searching for simple points in the
         * image.
		*/
		enum t_simple_points_algorithm
		{
			/**
			 * Null algorithm.
			 */
			SIMPLE_POINTS_NULL = 0,
			/**
			 * Find simple points (left to right, top to bottom).
			 */
			SIMPLE_POINTS_FIND_SWEEP,
			/**
			 * Removes simple points (left to right, top to bottom).
			 */
			SIMPLE_POINTS_REMOVE_SWEEP,
			/**
			 * Removes simple points that are in the borders of black components.
             *
             * \note
             *       It is necessary to have been applied borders operation previously.
			 */
			SIMPLE_POINTS_REMOVE_BORDERS,
			/**
			 * Like SIMPLE_POINTS_REMOVE_BORDERS but not removing points that are
             * in MA set (so the squeleton that is generated will be more connected
             * than in SIMPLE_POINT_REMOVE_BORDERS).
             *
             * \note
             *       MA set contains points of the image that have a MAT local
             *       maximum.
			 */
			SIMPLE_POINTS_REMOVE_BORDERS_NO_MA,
			/**
			 * Removes simple points that are north frontier of a black component.
			 */
			SIMPLE_POINTS_REMOVE_NORTH_FRONTIER,
			/**
			 * Removes simple points that are east frontier of a black component.
			 */
			SIMPLE_POINTS_REMOVE_EAST_FRONTIER,
			/**
			 * Removes simple points that are south frontier of a black component.
			 */
			SIMPLE_POINTS_REMOVE_SOUTH_FRONTIER,
			/**
			 * Removes simple points that are west frontier of a black component.
			 */
			SIMPLE_POINTS_REMOVE_WEST_FRONTIER,
			/**
             * Remove the simple points wich meets RH algorithm conditions and are north frontier,
             * to simulate this stage of the iterations in the RH parallel shrink algorithm.
			 */
			SIMPLE_POINTS_REMOVE_RH_NORTH,
			/**
             * Remove the simple points wich meets RH algorithm conditions and are east frontier,
             * to simulate this stage of the iterations in the RH parallel shrink algorithm.
			 */
			SIMPLE_POINTS_REMOVE_RH_EAST,
			/**
             * Remove the simple points wich meets RH algorithm conditions and are south frontier,
             * to simulate this stage of the iterations in the RH parallel shrink algorithm.
			 */
			SIMPLE_POINTS_REMOVE_RH_SOUTH,
			/**
             * Remove the simple points wich meets RH algorithm conditions and are west frontier,
             * to simulate this stage of the iterations in the RH parallel shrink algorithm.
			 */
			SIMPLE_POINTS_REMOVE_RH_WEST,
            /**
             * Remove simple points that meets ROS algorithm conditions and are north frontier,
             * to simulate this stage of the iterations in the ROS parallel shrink algorithm.
             */
            SIMPLE_POINTS_REMOVE_ROS_NORTH,
            /**
            * Remove simple points that meets ROS algorithm conditions and are east frontier,
            * to simulate this stage of the iterations in the ROS parallel shrink algorithm.
            */
            SIMPLE_POINTS_REMOVE_ROS_EAST,
            /**
            * Remove simple points that meets ROS algorithm conditions and are south frontier,
            * to simulate this stage of the iterations in the ROS parallel shrink algorithm.
            */
            SIMPLE_POINTS_REMOVE_ROS_SOUTH,
            /**
            * Remove simple points that meets ROS algorithm conditions and are west frontier,
            * to simulate this stage of the iterations in the ROS parallel shrink algorithm.
            */
            SIMPLE_POINTS_REMOVE_ROS_WEST,
			/**
			 * Remove the point wich meets GH89A1 algorithm conditions in odd iterations,
             * to simulate this step of the parallel shrink algorithm.
			 */
			SIMPLE_POINTS_REMOVE_GH89A1_ODD,
			/**
             * Remove the point wich meets GH89A1 algorithm conditions in even iterations,
             * to simulate this step of the parallel shrink algorithm.
			 */
			SIMPLE_POINTS_REMOVE_GH89A1_EVEN,
			/**
			 * Remove the point wich meets TSIN algorithm conditions in odd iterations,
             * to simulate this step of the parallel shrink algorithm.
			 */
			SIMPLE_POINTS_REMOVE_TSIN_ODD,
			/**
             * Remove the point wich meets TSIN algorithm conditions in even iterations,
             * to simulate this step of the parallel shrink algorithm.
			 */
			SIMPLE_POINTS_REMOVE_TSIN_EVEN,
			/**
             * Remove the point wich meets GH89A2 algorithm conditions in odd iterations,
             * to simulate this step of the parallel shrink algorithm.
			 */
			SIMPLE_POINTS_REMOVE_GH89A2_ODD,
			/**
             * Remove the point wich meets GH89A2 algorithm conditions in even iterations,
             * to simulate this step of the parallel shrink algorithm.
			 */
			SIMPLE_POINTS_REMOVE_GH89A2_EVEN
		};
		/**
         * Represents the status of the digital image.
         *
         * Some algorithms have dependencies on the results of other algorithms,
         * so the digital image status contains the algorithms applied previously.
         *
         * Status of the image can be the combination of some of them:
		 *
		 *    im_status = STATUS_LOADED | STATUS_WHITE_FRAME | STATUS_BLACK_COMPONENTS
		 *
		 *    This means that we have loaded the image from a bitmap file,
         *    created a white frame around the original image (which is a precondition
         *    for most of the algorithms),
         *    and searched black components.
		 */
		enum t_status
		{
			/**
			 * NULL image loaded. This happens when loading from a bitmap fails.
             *
             * \note
             *       This kind of image has not dimmensions, so we can't apply
             *       any algorithm to a image with this status.
			 */
			STATUS_NULL = 1,
			/**
             * Image correctly loaded from bitmap file.
             */
			STATUS_LOADED = 2,
			/**
			 * Blank image generated.
             *
             * \note
             *       This image has dimmensions, but all its points are white.
			 */
			STATUS_EMPTY = 4,
			/**
			 * Black frame created around the image.
			 */
			STATUS_BLACK_FRAME = 8,
			/**
			 * White frame created around the image.
             *
             * \note
             *       Most of the algorithms have this status as a precondition.
			 */
			STATUS_WHITE_FRAME = 16,
			/**
			 * Connected black components have been searched, and its points
             * are labelled with positive integers [1..number_of_component].
			 */
			STATUS_BLACK_COMPONENTS = 32,
			/**
			 * Connected white components have been searched, and its points
             * are labelled with negative integers [-1..(-number_of_components)].
			 */
			STATUS_WHITE_COMPONENTS = 64,
			/**
			 * Black components borders have been searched. Black points are
             * labelled with '1', and borders with '2'.
			 */
			STATUS_BORDER_BLACK_COMPONENTS = 128,
			/**
			 * Medial Axis Transformation (MAT) of the image have been calculated:
             * each black point of the image is labelled with its distance
             * to a white component (distance to the border + 1),
             * and white points labels are untouched.
			 */
			STATUS_MAT = 256,
			/**
			 * MA of the image have been calculated (points that have a local
             * maximum MAT).
			 */
			STATUS_MA = 512,
			/**
			 * Simple points have been searched and added to a list
             * (simple points found list).
			 */
			STATUS_SIMPLE_POINTS_FOUND = 1024,
			/**
			 * Simple points have been removed from the image and added to a list
             * (simple points removed list).
			 */
			STATUS_SIMPLE_POINTS_REMOVED = 2048
		};
		/**
		* Represents the four kind of frontier that a point could be.
		*/
		enum t_frontier
		{
			/**
			 * North frontier.
			 */
			FRONTIER_NORTH,
			/**
			 * East frontier.
			 */
			FRONTIER_EAST,
			/**
			 * South frontier.
			 */
			FRONTIER_SOUTH,
			/**
			 * West frontier.
			 */
			FRONTIER_WEST
		};
		/**
		 * Represents the parallel shrink algorithms that can be applied to the
         * image.
         * 
         * \note
         *       To add more parallel shrink algorithms you must add them here.
		 */
		enum t_parallel_shrink_algorithm
		{
			/**
			 * Null algorithm that makes nothing.
			 */
			PARALLEL_SHRINK_ALGORITHM_NULL = 0,
			/**
             * Removes north frontier simple points.
             *    - Steps of an iteration: 4
             *         - Step 1: removes simple points which are north frontier
             *                   of a black component.
             *         - Step 2: removes simple points which are east frontier
             *                   of a black component.
             *         - Step 3: removes simple points which are west frontier
             *                   of a black component.
             *         - Step 4: removes simple points which are south frontier
             *                   of a black component.
             */
            PARALLEL_SHRINK_ALGORITHM_FRONTIERS,
			/**
			 * RH algorithm.
             *    - Steps of an iteration: 4
             *         - Step 1: removes simple points which meets RH(north frontier)
             *                   conditions.
             *         - Step 2: removes simple points which meets RH(east frontier)
             *                   conditions.
             *         - Step 3: removes simple points which meets RH(south frontier)
             *                   conditions.
             *         - Step 4: removes simple points which meets RH(west frontier)
             *                   conditions.
			 */
			PARALLEL_SHRINK_ALGORITHM_RH,
			/**
            * ROS algorithm.
            *    - Steps of an iteration: 4
            *         - Step 1: removes simple points which meets ROS(north frontier)
            *                   conditions.
            *         - Step 2: removes simple points which meets ROS(east frontier)
            *                   conditions.
            *         - Step 3: removes simple points which meets ROS(south frontier)
            *                   conditions.
            *         - Step 4: removes simple points which meets ROS(west frontier)
            *                   conditions.
            */
			PARALLEL_SHRINK_ALGORITHM_ROS,
			/**
			 * GH89-A1 algorithm.
             *    - Odd iterations:
             *         - Step 1: removes simple points which meets GH89A1_ODD
             *                   conditions.
             *    - Even iterations:
             *         - Step 1: removes simple points which meets GH89A1_EVEN
             *                   conditions.
			 */
			PARALLEL_SHRINK_ALGORITHM_GH89A1,
			/**
			 * TSIN algorithm.
             *    - Odd iterations:
             *         - Step 1: removes simple points which meets TSIN_ODD
             *                   conditions.
             *    - Even iterations:
             *         - Step 1: removes simple points which meets TSIN_EVEN
             *                   conditions.            
			 */
			PARALLEL_SHRINK_ALGORITHM_TSIN,
			/**
			 * GH89-A2 algorithm.
             *    - Odd iterations:
             *         - Step 1: removes simple points which meets GH89A2_ODD
             *                   conditions.
             *    - Even iterations:
             *         - Step 1: removes simple points which meets GH89A2_EVEN
             *                   conditions.            
			 */
			PARALLEL_SHRINK_ALGORITHM_GH89A2
		};
		/**
		* Represents the way a matrix image will be shown in screen.
        *
        * \warning
        *          This is only for debug, use the output to file instead.
		*/
		enum t_screen_output
		{
			/**
			 * The image matrix will be shown using Unicode characters 
             *
             * \note
             *       Two matrix rows are printed using one screen line and
             *       one matrix col is printed using one screen char.
             *
             * \warning
             *          This is only for debug because the image could be
             *          so big for a typical screen.
			 */
			SCREEN_OUTPUT_ASCII,
			/**
             * The image matrix will be shown using the labels of the points:
             *    - >0 : black points (01, 02, ...)
             *    - <=0 : white points (00, -1, -2, ...)
             *
             * \note
             *       One matrix row is printed using one screen line and
             *       one matrix col is printed using two screen chars.
             *
             * \warning
             *          This is only for debug because the image could be
             *          so big for a typical screen, and if one image point
             *          label>9 or -9<label, then the output will be ugly.
			 */
			SCREEN_OUTPUT_COMPONENTS
		};
		/**
		 * Represents an error or exception.
		 */
		enum t_error
		{
			/**
			 * Problem when allocating memory for some structure.
			 */
			ERROR_MEMORY_PROBLEM = -10
		};
		/*
		 * Copy constructor.
		 *
		 * \pre
		 *      True.
		 *
		 * \post
		 *       Creates an image which is a copy of the image object
		 *       provided as argument.
		 *
		 * \return
		 *         Image object that is an exact copy of the one given
		 *         as argument.
		 */
		image (const topdig::image &image);
		/**
		 * Default constructor.
		 *
		 * \pre
		 *      True.
		 *
		 * \post
		 *      Creates a null image.
		 *
		 * \return
		 *         Image object with STATUS_NULL status. It will not be
		 *         allowed to apply algorithms to this image.
         *
         * \warning
         *          Do not use this constructor, it is intended to don't allow
         *          this kind of instantiation.
		 */
		image ();
		/**
		 * Constructor: read the image from a bitmap file.
		 *
		 * \pre
		 *      True.
		 *
		 * \post
		 *      Reads the image matrix from the bitmap file, which must be in
		 *      BMP format with the following restrictions:
		 *         - no compression (BI_RGB in BMP documentation).
		 *         - bpp: 24 or 32 (even if the image is a b&w picture it must
         *           be stored using 24 or 32 bpp).
         *      If there is NO error: im_status = STATUS_LOADED
         *      If there is an error: im_status = STATUS_NULL
		 *
		 * \param[in] filename
		 *        Filename of the bitmap file which must include the complete
         *        path of it.
		 *
		 * \param[in] color_format
		 *        Format to read the colour data por each point.
		 *
		 * \param[in] debug
		 *        Set the debug mode for the image object so there will be a lot
         *        of info printed to screen when applying the algorithms.
		 *
		 * \return
		 *         Image object which represents the image in the bitmap file.
		 */
		image (
			const std::string filename,
            const bool debug = false,
			const t_load_format color_format = LOAD_BW);
		/**
		 * Blank image constructor.
		 *
		 * \pre
		 *      True.
		 *
		 * \post
		 *      Creates a blank image with dimmensions provided and all its
         *      points in white.
         *      im_status = STATUS_BLANK
		 *
		 * \param[in] width
		 *        Width of the image (in points).
		 *
		 * \param[in] heigth
		 *        Height of the image (in points).
		 *
		 * \param[in] debug
         *        Set the debug mode for the image object so there will be a lot
         *        of info printed to screen when applying the algorithms.
		 *
		 * \return
		 *         Image object which represents a blank image of the dimmensions
         *         given.
		 */
		image (
			const dword width,
			const dword heigth,
			const bool debug = false);
		/**
		 * Destructor
		 *
		 * Frees all the memory used by the image, and delete it.
         *
         * \note
         *       Remember to save the content of the image to a bitmap file 
         *       before ;)
		 */
		~image();
		/**
		 * Stores the image in a bitmap file.
		 *
		 * \pre
		 *      True.
		 *
		 * \post
		 *      Creates a bitmap file that represents the data of the image
         *      object in the filename given as argument.
		 *
		 * \param[in] filename
		 *        Filename of the file in which store the bitmap (must include
         *        the full path).
		 *
		 * \param[in] output_format
		 *        Format used to translate image points labels to colours.
         *        By default SAVE_BW.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
         *         or 'false' if there is an error.
		 */
		bool save (
			const std::string filename,
			const t_save_format output_format = SAVE_BW);
		/**
		 * Prints the image matrix in screen.
		 *
		 * \pre
		 *      True.
		 *
		 * \post
		 *      Prints the image matrix to screen. This should be used only
         *      for debug, and with small images.
		 *
		 * \param[in] output_format
		 *        Format that will be used to translate matrix points labels to
         *        screen characters.
		 *        By default SCREEN_OUTPUT_ASCII.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
         *         of 'false' if there is an error.
		 */
		void print (
			const t_screen_output output_format = SCREEN_OUTPUT_ASCII);
		/**
		 * Prints the matrix given as argument instead of the image matrix in sreen.
		 *
		 * \pre
         *      'matrix' has the same dimmensions as image matrix (im_height x im_width).
		 *
         * \post
         *      Prints 'matrix' to screen. This should be used only
         *      for debug, and with small images.
         *
         * \param[in] output_format
         *        Format that will be used to translate matrix points labels to
         *        screen characters.
         *        By default SCREEN_OUTPUT_ASCII.
         *
         * \return
         *         Boolean 'true' if all goes good,
         *         of 'false' if there is an error.
		 */
		void print (
			t_colour ** matrix,
			const t_screen_output output_format = SCREEN_OUTPUT_ASCII);
		/**
		 * Returns the status of the image.
		 *
		 * \pre
		 *      True.
		 *
		 * \post
		 *      Returns the status of the image.
		 *
		 * \return
		 *         im_status
		 */
		t_status status (
			void);
		/**
		 * Creates a black frame around the image.
		 *
		 * \pre
		 *      True.
		 *
		 * \post
         *      - Creates a black frame (border) of the given width around the original
         *        image. The image dimmensions will grow by 2*width points.
         *        The original points of the image will be untouched, and the new
         *        points will be labelled with positiver integer '1'.
         *
         *      - im_status = (im_status & not STATUS_BLACK_COMPONENTS) | STATUS_BLACK_FRAME
         *        Which means that previous calls to black_components needs to be
         *        re-applied because black components of the image could have changed.
		 * 
		 * \param[in] width
		 *        Width of the frame. By default 1.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
         *         or 'false' if there is an error.
		 */
		bool frame_black (
			const dword width = (dword) 1);
		/**
		 * Creates a white frame around the image.
		 *
		 * \pre
		 *      True.
		 *
		 * \post
         *      - Creates a white frame (border) of the given width around the original
         *        image. The image dimmensions will grow by 2*width points.
         *        The original points of the image will be untouched, and the new
         *        points will be labelled with '0'.
         *
         *      - im_status = (im_status & not STATUS_WHITE_COMPONENTS) | STATUS_WHITE_FRAME
         *        Which means that previous calls to black_components needs to be
         *        re-applied because black components of the image could have changed.
		 * 
		 * \param[in] width
		 *        Width of the frame. By default 1.
		 *
		 * \return
         *         Boolean 'true' if all goes good,
         *         or 'false' if there is an error.
		 */
		bool frame_white (
			const dword width = (dword) 1);
		/**
		 * Indentifies black connected components.
         *
         * Search the number of black connected components of the image and
         * label each component with a different positive number. All points
         * of a component will have the same label, and two points of different
         * components will have different labels.            
		 *
		 * \pre
		 *      (im_status & STATUS_WHITE_FRAME) != 0
		 *
		 * \post
		 *      - Calls to components() and updates the number of black components
         *        with the value returned by the function. Different components
         *        has different labels, and a component has all of its points
         *        labelled with the same number.
         *
         *      - im_status = im_status | STATUS_BLACK_COMPONENTS
         *
         *      - im_black_components = Number of different connected black
         *        components found in the image.
		 *
		 * \param[in] adjacence
		 *        Adjacence that will be used. By default ADJ_8B_4W.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
         *         of 'false' if there is an error.
		 */
		bool black_components (
			const t_adjacence adjacence = ADJ_4B_8W);
		/**
         * Indentifies white connected components.
         *
         * Search the number of white connected components of the image and
         * label each component with a different negative number. All points
         * of a component will have the same label, and two points of different
         * components will have different labels.
         *
         * \pre
         *      (im_status & STATUS_WHITE_FRAME) != 0
         *
         * \post
         *      - Calculates the inverse image.
         *      - Calls to components() and updates the number of white components
         *        with the value returned by the function. Different components
         *        has different labels, and a component has all of its points
         *        labelled with the same number.
         *      - Calculates the inverse image.
         *
         *      - im_status = im_status | STATUS_WHITE_COMPONENTS
         *
         *      - im_white_components = Number of different connected white
         *        components found in the image.
         *
         * \param[in] adjacence
         *        Adjacence that will be used. By default ADJ_8B_4W.
         *
         * \return
         *         Boolean 'true' if all goes good,
         *         of 'false' if there is an error.
         */
		bool white_components (
			const t_adjacence adjacence = ADJ_4B_8W);
		/**
		 * Identifies the border points of the black components.
         *
         * The algorithm searches the borders of the black components.
         * There are two posible borders: a border that is a 8-curve, and
         * a border that is a 4-curve (which stands for 8-adjacent and
         * 4-adjacent points that makes the border curve).
		 *
		 * \pre
		 *      (im_status & STATUS_WHITE_FRAME) != 0
		 *
		 * \post
		 *      - Label the border points of the components with positive integer
         *        '2' and the rest of the black points with '1'. White points
         *        keeps their original labels.
         *
         *      - im_status = im_status | STATUS_BORDER_BLACK_COMPONENTS
		 *
		 * \param[in] adjacence
		 *        Adjacence of the border the algorith must find. By default ADJ_8B_4W.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
         *         or 'false' if there is an error.
		 *
		 * \sa frame_white()
		 */
		bool black_components_edges (
			const t_adjacence adjacence = ADJ_4B_8W);
		/**
		 * Calculates the Media Axis Transformation (MAT) of the image.
		 *
		 * The MAT consists on labelling each point of the image with its
		 * distance to the nearest white component (distance to the border
         * of the component + 1).
		 *
		 * \pre
		 *      (im_status & STATUS_WHITE_FRAME) != 0
		 *
		 * \post
		 *      - Labels each black point of the image with its distance to the
         *        nearest white component.
         *
         *      - White points are untouched.
         *
         *      - im_status = im_status | STATUS_MAT
		 *
		 * \param[in] adjacence
		 *        Adjacence used to measuring distance. By default ADJ_4B_8W.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
         *         'false' if there is an error.
		 *
		 * \sa frame_white()
		 */
		bool medial_axis_transformation (
			const t_adjacence adjacence = ADJ_4B_8W);
		/**
		 * Generates the MA of the image.
         *
         * Generates the MA of the image, that is a set of points for which
         * their MAT is a maximum local.
		 *
		 * \pre
		 *      (im_status & STATUS_MAT) != 0
		 *
		 * \post
		 *      - Clears the current MA set and genertates the new MA of the
         *        image.
         *
         *      - im_status = im_status | STATUS_MA
		 *
		 * \param[in] adjacence
		 *        Adjacence used in the algorithm. By default ADJ_4B_8W.
		 *
		 * \return
		 *         Boolean 'true' is all goes good,
         *         or 'false' if there is an error.
		 */
		bool ma (
			const t_adjacence adjacence = ADJ_4B_8W);
		/**
		 * Rebuilds the original image from its MA.
         *
         * MA is the minimum set of points from which we can rebuild
         * the original image without loosing any information.
         * MA contains a set of points and its distance to the border, so
         * we can draw balls with those centers and radius to rebuild the
         * original image (note that points that are not in the MA are
         * inside of some ball).
		 *
		 * \pre
		 *      (im_status & STATUS_MA) != 0
		 *
		 * \post
		 *       - Clears the image matrix.
         *
         *       - Rebuilds the original image matrix from the data of MA.
		 *
		 * \param[in] adjacence
		 *        Adjacence that will be used for the ball drawing when
         *        rebuilding the original image. By default ADJ_8B_4W.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
         *         or 'false' if there is an error.
		 */
		bool rebuild_from_ma (
			const t_adjacence adjacence = ADJ_8B_4W);
		/**
		 * Finds or removes simple points of the image.
		 *
		 * \pre
         *      - (im_status & STATUS_WHITE_FRAME) != 0
         *
		 *      - if algorithm == SIMPLE_POINTS_REMOVE_BORDER:
         *           - (im_status & STATUS_BORDER) != 0
         *
         *      - if algorithm == SIMPLE_POINTS_REMOVE_BORDER:
         *           - ((im_status & STATUS_BORDER) & STATUS_MA) != 0
		 *
		 * \post
		 *      - Finds or removes simples points of the image using the algorithm
         *        given as argument.
         *
         *      - if algorithm == SIMPLE_POINTS_FIND_SWEEP:
         *           - im_status = im_status | STATUS_SIMPLE_POINTS_FOUND
         *      - if algorithm == other
         *           - im_status = im_status | STATUS_SIMPLE_POINTS_REMOVED
		 *
		 * \param[in] algorithm
		 *        Algorithm that will be used. Note that some of them only finds
         *        simple points, but other finds and removes them from the image.
		 *
		 * \param[in] adjacence
		 *        Adjacence used in the algorithm that need it. By default ADJ_8B_4W.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
         *         or 'false' if there is an error.
		 */
		bool simple_points (
			const t_simple_points_algorithm algorithm,
			const t_adjacence adjacence = ADJ_8B_4W);
		/**
		 * Sequential shrink of the image.
         *
         * Sequential shrink of the image by removing simple points that meets
         * some condition given in the algorithm.
         *
         * \note
         *       This operation applies the algorithm given as argument until
         *       there are no more simple points in the image. You can simulate
         *       this operation by calling simple_points member function
         *       repeatly to see the changes between steps.
		 *
		 * \pre
         *      - (im_status & STATUS_WHITE_FRAME) != 0
         *
		 *      - if algorithm == SIMPLE_POINTS_REMOVE_BORDER:
         *           - (im_status & STATUS_BORDER) != 0
         *
         *      - if algorithm == SIMPLE_POINTS_REMOVE_BORDER:
         *           - ((im_status & STATUS_BORDER) & STATUS_MA) != 0
		 *
		 * \post
		 *      - Applies the algorithm given as argument until there are no
         *        more simple points in the image.
         *
         *      - im_status = im_status | STATUS_SIMPLE_POINTS_REMOVED
		 *
		 * \param[in] algorithm
		 *        Algorithm that will be used in the sequential shrink.
		 *
		 * \param[in] adjacence
		 *        Adjacence that will be used in the algorithms. By default ADJ_8B_4W.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
         *         or 'false' if there is an error.
		 */
		bool sequentiall_shrink (
			const t_simple_points_algorithm algorithm,
			const t_adjacence adjacence = ADJ_8B_4W);
		/**
		 * Parallel shrink of the image.
		 *
         * Applies a parallel shrink algorithm to the image, so the resulting
         * image is topologicaly equivalent to the original (same number of
         * black and white components).
         * This operation uses a number of threads that will apply the
         * algorithm to the image concurrently, so we can see that each parallel
         * algorithm doesn't change the topology of the original image.
         * 
		 * \pre
         *      - (im_status & STATUS_WHITE_FRAME) != 0
         *
		 *      - if algorithm == SIMPLE_POINTS_REMOVE_BORDER:
         *           - (im_status & STATUS_BORDER) != 0
         *
         *      - if algorithm == SIMPLE_POINTS_REMOVE_BORDER:
         *           - ((im_status & STATUS_BORDER) & STATUS_MA) != 0
		 *
		 * \post
		 *      - Applies a parallel shrink algorithm to the image using a number
         *        of threads working concurrently.
         *
         *      - im_status = im_status | STATUS_SIMPLE_POINTS_REMOVED
		 *
		 * \param[in] algorithm
		 *        Algorithm that will be used in the parallel shrink.
		 *
		 * \param[in] adjacence
		 *        Adjacence that will be used in the algorithms that need it. By default ADJ_8B_4W.
		 *
         * \param[in] num_threads
         *        Number of threads to create. By default 4.
         *        \note Creating more threads will not lead to a faster applying of the algorithm.
         *              The speed of the algorithm, when threads are created, depends on the hardware
         *              so in a multicore or distributed environment it could be faster than
         *              sequential shrink, and in a typical one-processor one-core environment it will
         *              be equal or less faster. This implementation of parallel shrink algorithms with
         *              threads is meant to demonstrate that they will work in a real parallel environment.
		 * \return
		 *         Boolean 'true' if all goes good,
         *         or 'false' if there is an error.
		 */
		bool parallel_shrink (
			const t_parallel_shrink_algorithm algorithm,
			const word num_threads = 4);

	protected:
		/**
		 * Image width in points.
		 */
		dword im_width;
		/**
		 * Image height in points.
		 */
		dword im_height;
		/**
		 * Number of black connected components.
		 */
		byte im_black_components;
		/**
		 * Number of white connected components.
		 */
		byte im_white_components;
		/**
		 * Image status: operations applied to the image (not all, some operations
         * can overwrite results of other). This will be used as a precondition
         * to some algorithms.
		 */
		t_status im_status;
		/**
		 * Whether to print debug info to screen or not when applying algorithms.
		 */
		bool im_debug;
		/**
		 * Matrix that contains the image points:
         *   - im_image_matrix[i][j] > 0 : black point.
         *   - im_image_matrix[i][j] <= 0 : white point.
		 */
		t_colour **im_image_matrix;	
		/**
		 * Copy of the original image matrix used in parallel shrink algorithms.
		 */
		t_colour **im_pshrink_initial_image_matrix;
		/**
		 * Type that represents a mutex.
		 */
		typedef boost::mutex t_mutex;
		/**
		 * Parallel shrink algorithms mutex: wait orders.
		 *
         * Is used to make threads wait until the main process has set the operations they will
         * have to do.
		 */
		t_mutex im_pshrink_mutex_wait_orders;
		/**
		 * Parallel shrink algorithms mutex: wait completion.
		 *
		 * Is used to make the main process wait until threads have finished doing its job.
		 */
		t_mutex im_pshrink_mutex_wait_completion;
		/**
		 * Mutex to synchronize acces between threads to global object attributes.
		 */
		t_mutex im_pshrink_mutex_access_object;
		/**
		 * Mutex needed to print debug to screen, as iostream is not thread safe.
		 */
		t_mutex im_mutex_io;
		/**
		 * Type that represents a lock in a mutex.
		 */
		typedef t_mutex::scoped_lock t_lock;
        /**
         * Represents a barrier (to synchronize threads and main execution process)
         */
        typedef boost::barrier t_barrier;
		/**
		 * Synchronization primitive used to cause a thread to wait for orders.
		 */
		t_barrier * im_barrier_wait_orders;
		/**
		 * Synchronization primitive used to cause the main program wait for the threads to finish.
         * It is a barrier that makes all the threads (including the main execution process) wait
         * until the last thread calls the barrier.wait().
		 */
		t_barrier * im_barrier_wait_completion;
		/**
         * Group of threads that will apply the parallel shrink algorithms.
		 */
		boost::thread_group pshrink_threads_group;
		/**
		 * Represents the equivalence between two components of the image.
		 *
		 * When searching for components in the image, initially, the algorithm found a lot
         * of unreal components (some of them are equivalent). This type is used to
         * define an equivalence between two components.
		 */
		struct t_equiv
		{
			/**
			 * Label of the component.
			 */
			t_colour c1;
			/**
			 * Label of the second component, equivalent to the first one, that could be
             * optimized by changing its points labels to the first component one.
			 */
			t_colour c2;
		};
		/**
		 * Represents a point of the image. Used for set of points.
		 */
		struct t_point
		{
			/**
			 * Row of the point.
			 */
			dword i;
			/**
			 * Col of the point.
			 */
			dword j;
		};
		/**
		 * Represents a point of the MA of the image.
		 */
		struct t_ma_point
		{
			/**
			 * Row of the point.
			 */
			dword i;
			/**
			 * Colof the point.
			 */
			dword j;
			/**
			 * Distance of the point to the nearest white component (distance
             * to the border + 1). This will be used as the ball radius when
             * rebuilding the original image from the MA set of points.
			 */
			dword distance;
		};
		/**
		 * Order relation between components. Used in components set.
		 */
		struct t_equiv_order
		{
			/**
			 * Order relation between components.
			 *
			 * \param[in] eq1
			 *        Component 1.
			 *
			 * \param[in] eq2
			 *        Component 2.
			 *
			 * \return
			 *         Boolean 'true' if ( eq1 <= eq2 ),
			 *         or 'false' in any other case.
			 */
			bool operator () (t_equiv eq1, t_equiv eq2) const
			{
				return ((eq1.c2 < eq2.c2) || (eq1.c2==eq2.c2 && eq1.c1 < eq2.c1));
			}
		};
		/**
		 * Components set of the image.
		 */
		typedef std::set<t_equiv, t_equiv_order> t_components_set;
		/**
		 * Order relation between MA points. Used in MA set.
		 */
		struct t_ma_point_order
		{
			/**
			 * Order relation between MA points.
			 *
			 * \param[in] p1
			 *        MA point 1.
			 *
			 * \param[in] p2
			 *        MA point 2.
			 *
			 * \return
			 *         Boolean 'true' if ( p1 <= p2 ),
			 *         or 'false' in any other case.
			 */
			bool operator () (t_ma_point p1, t_ma_point p2) const
			{
				return ((p1.i < p2.i) || (p1.i == p2.i && p1.j < p2.j));
			}
		};
		/**
		 * Relation order between points. Used in points set.
		 */
		struct t_point_order
		{
			/**
			 * Order relation between points.
			 *
			 * \param[in] p1
			 *        Point 1.
			 *
			 * \param[in] p2
			 *        Point 2.
			 *
			 * \return
			 *         Boolean 'true' if ( p1 <= p2 ),
			 *         or 'false' in any other case.
			 */
			bool operator () (t_point p1, t_point p2) const
			{
				return ((p1.i < p2.i) || (p1.i == p2.i && p1.j < p2.j));
			}
		};
		/**
		 * Represents a MA set.
		 */
		typedef std::set<t_ma_point, t_ma_point_order> t_ma_set;
		/**
		 * Represents a points set.
		 */
		typedef std::set<t_point, t_point_order> t_points_set;
		/**
		 * MA set.
		 *
		 * Contains the MA of the image, which is the minimum set of points
         * of the original image that could be used to represent the
         * image without loosing information.
		 */
		t_ma_set im_ma;
		/**
		 * Equivalent components set.
		 *
         * Contains the labels of the components of the image.
         * This set will be optimized to minimize the number of components
         * so we use the minimum first positive numbers needed to label
         * them.
         * This is needed because the algorithms initially finds a lot of components
         * that are equivalent (because they search from lef to to right, top
         * to bottom, and the adjacence forces the discovering of new components
         * that are equivalent to other already found).
		 */
		t_components_set im_equivalent_components;
		/**
		 * Simple points found sed.
         *
         * Contains the simple points found in the past applications of
         * simple_points, sequential_shrink or parallel_shrink algorithms.
		 */
		t_points_set im_simple_points_found;
		/**
		 * Simple points removed sed.
         *
         * Contains the simple points removed in the past applications of
         * simple_points, sequential_shrink or parallel_shrink algorithms.
		 */
		t_points_set im_simple_points_removed;
		/**
		 * Represents the range in which a threat will apply the parallel shrink algorithm.
		 */
		struct t_points_range
		{
			/**
			 * Coordinates of the starting point of the range.
			 */
			t_point start;
			/**
			 * Coordinates of the ending point of the range.
			 */
			t_point end;
		};
		/**
		 * Represents the action that a thread must do in parallel shrink algorithms,
         * and the results of the work done by the thread.
		 */
		struct t_thread_action
		{
			/**
			 * Stores the algorithm that each the thread must apply to the image.
			 */
			t_simple_points_algorithm algorithm;
			/**
			 * Indicates if the thread has a range of the matrix assigned (to see
             * if a thread is doing anything or just sleeping).
			 */
			bool range_assigned;
			/**
			 * This is the range of the image matrix in which the thread
			 * will apply the algorithm.
			 */
			t_points_range range;
			/**
			 * Stores the coordinates in which the thread is applying the algorithm
			 * assigned. The thread will apply the algorithm to all the points if 
			 * the image matrix, from range.start to range.end.
			 */
			t_point pos;
			/**
			 * The thread stores here wheter or not it has found simple points in the
             * actual stage of the algorithm.
			 */
			bool simple_point_found;
		};
		/**
		* Represents the status of the parallel shrink algorithm process.
		*/
		struct t_threads_actions
		{
			/**
			 * Number of threads that are applying the algorithm.
			 */
			word num_threads;
			/**
			 * Algorithm that will be used in the parallel shrink of the image.
			 */
			t_parallel_shrink_algorithm algorithm;
			/**
			 * Current stage of the algorithm iteration.
			 */
			word stage;
            /**
             * Whether the algorithm has found simple points in the last iteration
             * \note One iteration of the algorithm can have various stages. The
             * stop condition is that no simple point have been found in a
             * complete iteration.
             */
            bool simple_points_found;
			/**
			 * Adjacence that will be used in the algorithms that use it.
             *
             * \note Current algorithms does not use this value because all of them
             * applies to a (8,4)-image.
			 */
			t_adjacence adjacence;
			/**
			 * Stores the information related to each thread working in the algorithm.
			 */
			t_thread_action * thread;
			/**
			 * Number of threads still working in the algorithm. This is used to
             * notify the main process that all threads have finished.
			 */
			dword threads_working;
		};
		/**
		 * Threads actions and status.
         *
         * In parallel shrink algorithms we need to have some threads working 
         * concurrently on the image. Here we store the status of the work
         * of each thread, and the global status of the algorithm.
		 */	
		t_threads_actions im_threads_actions;
		/**
		 * Creates a frame of the colour given outside the image.
		 *
		 * \pre
		 *      True.
		 *
		 * \post
		 *      - if frame_points_colour == COLOUR_BLACK: it calls 'frame_black()'.
		 *      - if frame_points_colour == COLOUR_WHITE: it calls 'frame_white()'.
		 * 
		 * \param[in] frame_points_colour
		 *        Colour for the points of the frame.
		 *
		 * \param[in] width
		 *        Width for the frame. By default 1 point.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
		 *         or 'false' if there is an error.
		 *
		 * \sa frame_black()
		 * \sa frame_white()
		 */
		bool frame_colour (
			const t_colour frame_points_colour,
			const dword width = (dword) 1);
		/**
		 * Labels each black component of the image with a different positive integer.
		 *
		 * \pre
		 *      (im_status & STATUS_WHITE_FRAME) != 0
		 *
		 * \post
		 *      - Search the image for black components and labels all the points of
		 *           each component with the same positive integer. The points of a component
		 *           will have the same label, and each component will have a different label
		 *           for its points. Labels are in range (1 .. number_of_black_components).
         *
		 *      - im_status = im_status | STATUS_BLACK_COMPONENTS
		 *
		 * \param[in] adjacence
		 *        Adjacence used for the image. By default ADJ_8B_4W.
		 *
		 * \return
		 *         Boolean 'true' if all goes good,
		 *         or 'false' in any other case.
		 *
		 * \sa frame_white()
		 */
		t_colour components (
			const t_adjacence adjacence);
		/**
		 * Returns the number of black adjacent points to a given one.
		 *
		 * \pre
		 *      Point coordinates are in range ( 1 .. im_height , 1 .. im_width ).
		 *
		 * \post
		 *      Return the number of black adjacent points to the given one,
		 *      depending on the adjacence used (given as a parameter).
		 *
		 * \param[in] i Row of the point..
		 *
		 * \param[in] j Col of the point.
		 *
		 * \param[in] adjacence Adjacence that will be used.
		 *
		 * \return
		 *         Positive integer that indicates the number of black adjacent points.
		 */
		byte black_adjacent_points (
			const dword i,
			const dword j,
			const t_adjacence adjacence);
		/**
		 * Returns the number of black adjacent points to the given one.
		 *
		 * \note This has the same behaviour but using the matrix given as argument,
		 * and NOT to the matrix of the image.
		 *
		 * \pre
		 *      Point coordinates are in range ( 1 .. im_height , 1 .. im_width ).
		 *
		 * \post
		 *      Return the number of black adjacent points to the given one,
		 *      depending on the adjacence used (given as a parameter).
		 *
		 * \param[in] i Row of the point..
		 *
		 * \param[in] j Col of the point.
		 *
		 * \param[in] adjacence Adjacence that will be used.
		 *
		 * \return
		 *         Positive integer that indicates the number of black adjacent points.
		 */
		byte black_adjacent_points (
			t_colour ** matrix,
			const dword i,
			const dword j,
			const t_adjacence adjacence);
		/**
		 * Determines if a point is in the frontier of a component.
		 *
		 * \pre
		 *      Point coordinates are in range ( 2 .. im_height-1 , 2 .. im_width-1 ).
		 *
		 * \post
		 *      Determines if a point is {north, east, south, west} frontier of
		 *      a component, depending on the adjacence.
		 *
		 * \param[in] i Row of the point in the matrix.
		 *
		 * \param[in] j Col of the point in the matrix.
		 *
		 * \param[in] frontier Frontier to use.
		 *
		 * \param[in] adjacence Adjacence to use.
		 *
		 * \return
		 *         Boolean 'true' if the point is a frontier of the kind and adjacence given,
		 *         or 'false' if it is not.
		 */
		bool is_frontier (
			const dword i,
			const dword j,
			const t_frontier frontier);
		/**
		 * Determines if a point is in the frontier of a component using 'matrix' instead of
		 * image original matrix.
		 *
		 * \note This is needed because in some algorithms changes are applied to the image
		 * matrix, but the conditions depends in another matrix (ie, an unmodified
		 * original matrix).
		 *
		 * \pre
		 *      Point coordinates are in range ( 2 .. im_height-1 , 2 .. im_width-1 ).
		 *
		 * \post
		 *      Determines if a point is {north, east, south, west} frontier of
		 *      a component, depending on the adjacence.
		 *
		 * \param[in] matrix Matrix in which look for the point.
		 *
		 * \param[in] i Row of the point in the matrix.
		 *
		 * \param[in] j Col of the point in the matrix.
		 *
		 * \param[in] frontier Frontier to use.
		 *
		 * \param[in] adjacence Adjacence to use.
		 *
		 * \return
		 *         Boolean 'true' if the point is a frontier of the kind and adjacence given,
		 *         or 'false' if it is not.
		 */
		bool is_frontier (
			image::t_colour ** matrix,
			const dword i,
			const dword j,
			const t_frontier frontier);
		/**
		 * Lee un byte desde file.
		 *
		 * \pre
		 *      El file esta abierto en modo lectura.
		 *
		 * \post
		 *      Devuelve la media word_to_write leida y mueve el puntero dentro del file de
		 *      forma que apunta al siguiente byte al leido.
		 *
		 * \param[in,out] file Fichero ya abierto del que leer.
		 *
		 * \return
		 *         Devuelve la media word_to_write situada en la posicion actual del cursor
		 *         dentro del file.
		 */
		byte read_byte (
			std::fstream& file);
		/**
		 * Lee una palabra desde file.
		 *
		 * \pre
		 *      El file esta abierto en modo lectura.
		 *
		 * \post
		 *      Devuelve la word_to_write leida y mueve el puntero dentro del file
		 *      de forma que se salte los dos bytes leidos.
		 *
		 * \param[in,out] file Fichero ya abierto del que leer.
		 *
		 * \return
		 *         Devuelve la word_to_write formada por los dos
		 *         bytes leidos teniendo en cuenta que estan
		 *         almacenados en formato little-endian en el file.
		 */
		word read_word (
			std::fstream& file);
		/**
		 * Lee una doble palabra desde file.
		 *
		 * \pre
		 *      El file esta abierto en modo lectura.
		 *
		 * \post
		 *      Devuelve la doble word_to_write leida y mueve el puntero dentro del file
		 *      de forma que se salte los cuatro bytes leidos.
		 *
		 * \param[in,out] file Fichero ya abierto del que leer.
		 *
		 * \return
		 *         Devuelve la doble word_to_write formada por los cuatro
		 *         bytes leidos teniendo en cuenta que estan
		 *         almacenados en formato little-endian en el file.
		 */
		dword read_dword (
			std::fstream& file);
		/**
		 * Extrae la media palabra indicada de la doble word_to_write dada.
		 *
		 * \pre
		 *      Cierto
		 *
		 * \post
		 *      Dada una doble word_to_write y el index de un byte dentro de ella,
		 *      extrae el byte correspondiente y lo devuelve.
		 *
		 * \param[in] double_word
		 *        Doble word_to_write de la que extraer el byte indicado.
		 *
		 * \param[in] half_word
		 *        Byte a extraer de la doble word_to_write dada.
		 *
		 * \return
		 *         Devuelve el byte pedido.
		 */
		dword extract_byte_from_dword (
			const dword double_word,
			const unsigned short int half_word);  
		/**
		 * Escribe una media palabra en el file.
		 *
		 * \pre
		 *      El file esta abierto en modo escritura y se pueden anadir datos.
		 *
		 * \post
		 *      Anade la media word_to_write indicada al final del file.
		 *
		 * \param[in,out] file Fichero ya abierto en el que escribir.
		 *
		 * \param[in] byte_to_write Byte a anadir al file.
		 *
		 * \return
		 *         Devuelve 'true' si todo va bien
		 *         o 'false' en cualquier otro caso.
		 */
		bool write_byte (
			std::fstream& file,
			const byte byte_to_write);
		/**
		 * Escribe una palabra en el file.
		 *
		 * \pre
		 *      El file esta abierto en modo escritura y se pueden anadir datos.
		 *
		 * \post
		 *      Anade la word_to_write indicada al final del file,
		 *      almacenandola por bytes en formato little-endian.
		 *
		 * \param[in,out] file Fichero ya abierto en el que escribir.
		 *
		 * \param[in] word_to_write Palabra a anadir al file.
		 *
		 * \return
		 *         Devuelve 'true' si todo va bien
		 *         o 'false' en cualquier otro caso.
		 */
		bool write_word_lendian (
			std::fstream& file,
			const word word_to_write);
		/**
		 * Escribe una doble word_to_write en el file.
		 *
		 * \pre
		 *      El file esta abierto en modo escritura y se pueden anadir datos.
		 *
		 * \post
		 *      Anade la doble word_to_write indicada al final del file,
		 *      almacenandola por bytes en formato little-endian.
		 *
		 * \param[in,out] file Fichero ya abierto en el que escribir.
		 *
		 * \param[in] double_word Doble word_to_write a anadir al file.
		 *
		 * \return
		 *         Devuelve 'true' si todo va bien
		 *         o 'false' en cualquier otro caso.
		 */
		bool write_dword_lendian (
			std::fstream& file,
			const dword double_word);
		/**
		 * Codigo para los hilos de ejecucion de eliminacion paralela de puntos simples
		 *
		 * Este sera el codigo que ejecuten los hilos de ejecucion que se lanzaran
		 * durante la llamada a la operacion 'eliminacion paralela de puntos simples'.
		 * Cada hilo se ira bloqueando a la espera de una accion a realizar, ejecutara
		 * la accion y volvera a bloquearse a la espera de la siguiente. Esto es easi
		 * porque aunque se pueden aplicar algoritmos de forma paralela, el avance por
		 * los distintos pasos de los algoritmos tiene que estar sincronizado para
		 * conservar la topologia de la imagen original.
		 *
		 * \pre
		 *      El mutex de sincronizacion esta bloqueado antes de crear este hilo
		 *      de ejecucion.
		 *
		 * \post
		 *       Espera a que se le asigne una accion (espera en el bloqueo del mutex) y
		 *       la ejecuta. Si la accion es HILOS_FINALIZAR, termina su ejecucion.
		 *
		 * \param[in] thread_number
		 *        Especifica el numero del subproceso, ya que debe saber quien es el msimo
		 *        para modificar la posicion exacta de la estructura de datos que contiene
		 *        las acciones que deben llevar a cabo.
		 *
		 * \return
		 *         No devuelve ningun valor.
		 */
		static void pshrink_threads_main (
			image * object,
			word thread_number);
		/**
		 * Inicia la estructura de datos de los hilos de ejecucion de EPPS.
		 *
		 * Inicia la estructura de datos de los hilos de ejecucion del algoritmo
		 * de eliminacion paralela de puntos simples. Simplemente crea una
		 * estructura del tamano adecuado al numero de hilos que se crearan
		 * pero no asigna ninguna tarea.
		 *
		 * \pre
		 *      No esta iniciada la estructura de datos.
		 *
		 * \post
		 *       Inicia la estructura de datos reservando el espacio de memoria
		 *       necesario, dependiendo del numero de hilos que se van a crear.
		 *
		 * \param[in] algorithm
		 *        Algoritmo que se usara en la eliminacion de puntos simples.
		 *
		 * \param[in] adjacence
		 *        Adyacencia que se usara en el algoritmo.
		 *
		 * \param[in] num_threads
		 *        Numero de procesos que se crearan.
		 *
		 * \return
		 *         Devuelve 'true' en caso de exito,
		 *         o 'false' en caso de que haya algun problema con la reserva
		 *         de memoria.
		 */
		bool pshrink_init_structure (
			const t_parallel_shrink_algorithm algorithm,
			const word num_threads);
		/**
		 * Prepara la siguiente accion de los hilos de ejecicion de EPPS.
		 *
		 * Prepara el siguiente paso del algoritmo de eliminacion paralela de puntos
		 * simples que debe llevar a cabo cada uno de los hilos de ejecucion lanzados.
		 * La accion que llevara a cabo cada hilo de ejecucion dependera del paso
		 * actual del algoritmo, y del algoritmo en si mismo.
		 *
		 * \pre
		 *      Se ha iniciado la estructura de datos de los hilos de ejecucion.
		 *
		 * \post
		 *       Asigna una accion a cada hilo de ejecucion.
		 *
		 * \return
		 *         Devuelve 'true' en caso de que haya podido asignar correctamente
		 *         acciones a cada hilo de ejecucion,
		 *         o 'false' en caso de que se haya terminado de aplicar el
		 *         algoritmo y se haya mandado la orden de finalizacion a los hilos.
		 */
		void pshrink_set_actions (void);
		/**
		 * Determines if the parallel algorithm has finished.
		 *
		 * \pre
		 *      There is a parallel shrink algorithm in use.
		 *
		 * \post
		 *      Determines whether the algorithm has finished. When a iteration of
		 *      the algorithm has not found simple points, the algorithm
		 *      is finished. A iteration of the algorithm can have some stages
		 *      (ie, frontiers algorithm has four stages: north frontiers, east...)
		 *      so it is necessary to not have found simple points in all stages
		 *      of an iteration.
		 *
		 * \return
		 *      Boolean 'true' if the algorithm has finished,
		 *      or 'false' if not.
		 */
		bool pshrink_continue_condition (void);
		/**
		 * Change to next stage of the current parallel shrink algorithm.
		 *
		 * \pre
		 *      Parallel algorithm is been applied.
		 *
		 * \post
		 *       Change algorithm to its next stage of the current iteration.
		 */
		void pshrink_algorithm_next_stage(void);
		/**
		 * Updates algorithms-condition-matrix with changes after finishing an iteration of the algorithm
		 *
		 * \pre
		 *      Parallel algorithm is been applied.
		 *
		 * \post
		 *       Update all removed points in the matrix used for testing conditions in
		 *       parallel shrink algorithms. This is made only after a complete iteration
		 *       of the algorithm finishes (not every stage of a iteration, because in
		 *       this case, topology of the original image could be altered).
		 */
		void pshrink_end_iteration_update_matrix(void);
		/**
		 * Determines if an iteration of the algorithm has finished.
		 *
		 * \pre
		 *      Parallel algorithm is been applied.
		 * 
		 * \post
		 *       Determines whether an iteration is finished, or ifthere are more stages remaining.
		 */
		bool pshrink_test_if_iteration_end(void);
		/**
		 * Determines if a point is a 'simple point'
		 *
		 * This member function look at the adjacent points of a given one, and
		 * determines if that point is a simple point.
		 * A simple point is a point of a digital image if its removing does not
		 * modifies the topology of the original image.
		 *
		 * \pre
		 *       True.
		 *
		 * \post
		 *       Determines if a given point is a simple point.
		 *
		 * \param[in] i
		 *        Row of the point to test.
		 *
		 * \param[in] j
		 *        Col of the point to test.
		 *
		 * \return
		 *         Boolean 'true' if the point (i,j) is a simple point,
		 *         or 'false' if it is not a simple point.
		 */
		bool is_simple_point (
			const dword i,
			const dword j,
			const t_adjacence adjacence);
		/**
		 * Determines if a point is a 'simple point' using the matrix given instead
		 * of the image matrix.
		 *
		 * This member function look at the adjacent points of a given one, and
		 * determines if that point is a simple point.
		 * A simple point is a point of a digital image if its removing does not
		 * modifies the topology of the original image.
		 *
		 * \pre
		 *       True.
		 *
		 * \post
		 *       Determines if a given point is a simple point.
		 *
		 * \param[in] matrix
		 *        Matrix in which test the conditions.
		 * 
		 * \param[in] i
		 *        Row of the point to test.
		 *
		 * \param[in] j
		 *        Col of the point to test.
		 *
		 * \return
		 *         Boolean 'true' if the point (i,j) is a simple point,
		 *         or 'false' if it is not a simple point.
		 */
		bool is_simple_point (
			t_colour **matrix,
			const dword i,
			const dword j,
			const t_adjacence adjacence);
		/**
		 * Determines if a point meets the shrink RH algorithm conditions.
		 *
		 * This algorithm only applies to (8,4)-images.
		 *
		 * \pre
		 *       The image a (8,4)-image.
		 *
		 * \post
		 *       Determines if a point meets the shrink RH algorithm conditions, which are:
		 *          - p is a simple point.
		 *          - p is north frontier or N*8(p) == 1
		 *          - p surroundings ARE NOT ANY of the following:
		 *            \image html RH_condition_c.jpg
		 *
		 * \param[in] i
		 *        Row of the point.
		 *
		 * \param[in] j
		 *        Col of the point.
		 *
		 * \return
		 *         Boolean 'true' if the point meets the conditions,
		 *         or 'false' if not.
		 */
		bool meets_shrink_rh_conditions (
			const dword i,
			const dword j,
			t_frontier frontier = image::FRONTIER_NORTH);
		/**
		 * Determines if a point meets the shrink ROS algorithm conditions.
		 *
		 * This algorithm only applies to (8,4)-images.
		 *
		 * \pre
		 *       The image a (8,4)-image.
		 *
		 * \post
		 *       Determines if a point meets the shrink ROS algorithm conditions, which are:
		 *          - p is a simple point.
		 *          - p is north frontier.
         *          - p is not an extreme point of the component.
		 *          - p surroundings ARE NOT ANY of the following:
		 *            \image html RH_condition_c.jpg
		 *
		 * \param[in] i
		 *        Row of the point.
		 *
		 * \param[in] j
		 *        Col of the point.
		 *
		 * \return
		 *         Boolean 'true' if the point meets the conditions,
		 *         or 'false' if not.
		 */
		bool meets_shrink_ros_conditions (
			const dword i,
			const dword j,
			t_frontier frontier = image::FRONTIER_NORTH);
		/**
		 * Determines if a point meets the shrink GH89-A1 algorithm conditions.
		 *
		 * This algorithm only applies to (8,4)-images.
		 *
		 * \pre
		 *       The image a (8,4)-image.
		 *
		 * \post
		 *       Determines if a point meets the shrink GH89-A1 algorithm conditions, which are:
		 *          - N*(p) (intersection) N is 8-connected.
		 *          - not (
		 *                 p has one or two adjacent points and N*(p) (intersection) N is 4-connected
		 *                ).
		 *          - even iterations of the algorithm:
		 *                 N(p) IS ONE of the following: \image html gh89a1_condition_c1.jpg
		 *          - odd iterations of the algorithm:
		 *                 N(p) IS ONE of the following: \image html gh89a1_condition_c2.jpg
		 *
		 * \param[in] i
		 *        Row of the point.
		 *
		 * \param[in] j
		 *        Col of the point.
		 *
		 * \param[in] num_iteration
		 *
		 * \return
		 *         Boolean 'true' if the point meets the conditions,
		 *         or 'false' if not.
		 */
		bool meets_shrink_gh89a1_conditions (
			const dword i,
			const dword j,
			const byte num_iteration);
		/**
		 * Determines if a point meets the shrink TSIN algorithm conditions.
		 *
		 * This algorithm only applies to (8,4)-images.
		 *
		 * \pre
		 *       The image a (8,4)-image.
		 *
		 * \post
		 *       Determines if a point meets the shrink TSIN algorithm conditions, which are:
		 *          - N*(p) (intersection) N is 8-connected.
		 *          - not (
		 *                 p has one or two adjacent points and N*(p) (intersection) N is 4-connected
		 *                ).
		 *          - even iterations of the algorithm:
		 *                 N(p) is NOT one of the following: \image html tsin_condition_c1.jpg
		 *          - odd iterations of the algorithm:
		 *                 N(p) is NOT one of the following: \image html tsin_condition_c2.jpg
		 *
		 * \param[in] i
		 *        Row of the point.
		 *
		 * \param[in] j
		 *        Col of the point.
		 *
		 * \param[in] num_iteration
		 *
		 * \return
		 *         Boolean 'true' if the point meets the conditions,
		 *         or 'false' if not.
		 */
		bool meets_shrink_tsin_conditions (
			const dword i,
			const dword j,
			const byte num_iteration);
		/**
		 * Determines if a point meets the shrink GH89-A2 algorithm conditions.
		 *
		 * This algorithm only applies to (8,4)-images.
		 *
		 * \pre
		 *       The image a (8,4)-image.
		 *
		 * \post
		 *       Determines if a point meets the shrink GH89-A2 algorithm conditions, which are:
		 *          - p is a simple point.
		 *          - N*(p) (intersection) N has two or more points.
		 *
		 * \param[in] i
		 *        Row of the point.
		 *
		 * \param[in] j
		 *        Col of the point.
		 *
		 * \return
		 *         Boolean 'true' if the point meets the conditions,
		 *         or 'false' if not.
		 */
		bool meets_shrink_gh89a2_conditions (
			const dword i,
			const dword j);
		/**
		 * Determines if a set of points is 8-connected or 4-connected. 
		 *
		 * \pre
		 *       True.
		 *
		 * \post
		 *       Analyzes a set of points and determines if it is 8-connected or 4-connected
		 *       depending on the argument 'adjacence'.
		 *
		 * \param[in,out] points_set
		 *        Set of points to be analyzed.
		 *
		 * \param[in] adjacence
		 *        Determines if we want to know if the set is 8-connected (ADJ_8B_4W) or
		 *        4-connected (ADJ_4B_8W).
		 *
		 * \return
		 *         Boolean 'true' if the set is connected,
		 *         or 'false' if it is not connected.
		 */
		bool points_set_is_connected (
			t_points_set * points_set,
			const t_adjacence adjacence);
		/**
		 * Determines if a set of points is 8-connected or 4-connected using 'matrix' instead of
		 * image matrix to test the conditions.
		 *
		 * \pre
		 *       True.
		 *
		 * \post
		 *       Analyzes a set of points and determines if it is 8-connected or 4-connected
		 *       depending on the argument 'adjacence'.
		 *
		 * \param[in] matrix
		 *        Matrix in which the algorithm will test the conditions.
		 *
		 * \param[in,out] points_set
		 *        Set of points to be analyzed.
		 *
		 * \param[in] adjacence
		 *        Determines if we want to know if the set is 8-connected (ADJ_8B_4W) or
		 *        4-connected (ADJ_4B_8W).
		 *
		 * \return
		 *         Boolean 'true' if the set is connected,
		 *         or 'false' if it is not connected.
		 */
		bool points_set_is_connected (
			t_colour **matrix,
			t_points_set * points_set,
			const t_adjacence adjacence);
		/**
		 * Calculates the intersection between two points sets.
		 *
		 * \pre
		 *       True.
		 *
		 * \post
		 *       Creates a new set with the points of the intersection
		 *       of the two sets given.
		 *
		 * \param[in] set1
		 *        First set to intersect.
		 *
		 * \param[in] set2
		 *        Second set to intersect.
		 *
		 * \return
		 *         Result of the intersection.
		 */
		t_points_set * points_set_intersection (
		    const t_points_set &set1,
			const t_points_set &set2);
        /**
		 * Create the set of adjacent points to a given one.
		 *
		 * In some algorithms it is necessary to know the set of points
		 * adjacents to other. This is which we call N*(p):
		 * set of points adjacents to point 'p' without 'p' in the set.
		 * There are two possibilities for this set: N8*(p) or N4*(p) depending
		 * on the adjacence we use.
		 *
		 * \pre
		 *       True.
		 *
		 * \post
		 *       Creates the set of points N*(p).
		 *
		 * \param[in] i
		 *        Row of the point.
		 *
		 * \param[in] j
		 *        Col of the point.
		 *
		 * \param[in] adjacence
		 *        Type of adjacence:
		 *           - ADJ_8B_4W : the resulting set will be N8*(p).
		 *           - ADJ_4B_8W : the resulting set will be N4*(p).
		 *
		 * \return
		 *         The points set N*(p).
		 */
        t_points_set * points_set_create_black_adjacents (
		    const dword i,
			const dword j,
			const t_adjacence adjacence);
        /**
		 * Create the set of adjacent points to a given one using 'matrix'
		 * instead of matrix image to get data.
		 *
		 * In some algorithms it is necessary to know the set of points
		 * adjacents to other. This is which we call N*(p):
		 * set of points adjacents to point 'p' without 'p' in the set.
		 * There are two possibilities for this set: N8*(p) or N4*(p) depending
		 * on the adjacence we use.
		 *
		 * \pre
		 *       True.
		 *
		 * \post
		 *       Creates the set of points N*(p).
		 *
		 * \param[in] matrix
		 *        Matrix from which extract data.
		 *
		 * \param[in] i
		 *        Row of the point.
		 *
		 * \param[in] j
		 *        Col of the point.
		 *
		 * \param[in] adjacence
		 *        Type of adjacence:
		 *           - ADJ_8B_4W : the resulting set will be N8*(p).
		 *           - ADJ_4B_8W : the resulting set will be N4*(p).
		 *
		 * \return
		 *         The points set N*(p).
		 */
		t_points_set * points_set_create_black_adjacents (
			t_colour **matrix,
			const dword i,
			const dword j,
			const t_adjacence adjacence);
	};
}

#endif
