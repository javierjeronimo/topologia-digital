/**
 *
 * \file main.cpp
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

#include <iostream>
#include <fstream>
using namespace std;


#include "topologia_digital/operations.h"
#include "topologia_digital/image.h"

int main (int argc, char **argv)
{
	/* Variables que se usaran */
	operations *OP_image;
	string image_filename;
	string operations_filename;

	/* Mostramos la licencia si se pide en los argumentos */
	if (argc == 2 &&
			(strcmp (argv[1], "/licencia") == 0 ||
			 strcmp (argv[1], "/license") == 0 ||
			 strcmp (argv[1], "--licencia") == 0 ||
			 strcmp (argv[1], "--license") == 0 ))
	{
		cout << "Topologia Digital version 1 Copyright (C) 2006 Javier Carlos Jeronimo Suarez" << endl << endl;
		cout << "Author: Javier Carlos Jeronimo Suarez <jcjeronimo@alumnos.upm.es>" << endl;
		cout << "        Digital Topology Course" << endl;
		cout << "        2005-2006 Year" << endl;
		cout << "        Applied Mathematics Department" << endl;
		cout << "        Computer Science Faculty" << endl;
		cout << "        Polytechnic University of Madrid" << endl << endl;
		cout << "                    GNU GENERAL PUBLIC LICENSE" << endl;
		cout << "                       Version 2, June 1991" << endl;
		cout << "" << endl;
 		cout << " Copyright (C) 1989, 1991 Free Software Foundation, Inc.," << endl;
 		cout << " 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA" << endl;
 		cout << " Everyone is permitted to copy and distribute verbatim copies" << endl;
 		cout << " of this license document, but changing it is not allowed." << endl;
		cout << "" << endl;
		cout << "                            Preamble" << endl;
		cout << "" << endl;
		cout << "  The licenses for most software are designed to take away your" << endl;
		cout << "freedom to share and change it.  By contrast, the GNU General Public" << endl;
		cout << "License is intended to guarantee your freedom to share and change free" << endl;
		cout << "software--to make sure the software is free for all its users.  This" << endl;
		cout << "General Public License applies to most of the Free Software" << endl;
		cout << "Foundation's software and to any other program whose authors commit to" << endl;
		cout << "using it.  (Some other Free Software Foundation software is covered by" << endl;
		cout << "the GNU Lesser General Public License instead.)  You can apply it to" << endl;
		cout << "your programs, too." << endl;
		cout << "" << endl;
		cout << "  When we speak of free software, we are referring to freedom, not" << endl;
		cout << "price.  Our General Public Licenses are designed to make sure that you" << endl;
		cout << "have the freedom to distribute copies of free software (and charge for" << endl;
		cout << "this service if you wish), that you receive source code or can get it" << endl;
		cout << "if you want it, that you can change the software or use pieces of it" << endl;
		cout << "in new free programs; and that you know you can do these things." << endl;
		cout << "" << endl;
		cout << "  To protect your rights, we need to make restrictions that forbid" << endl;
		cout << "anyone to deny you these rights or to ask you to surrender the rights." << endl;
		cout << "These restrictions translate to certain responsibilities for you if you" << endl;
		cout << "distribute copies of the software, or if you modify it." << endl;
		cout << "" << endl;
		cout << "  For example, if you distribute copies of such a program, whether" << endl;
		cout << "gratis or for a fee, you must give the recipients all the rights that" << endl;
		cout << "you have.  You must make sure that they, too, receive or can get the" << endl;
		cout << "source code.  And you must show them these terms so they know their" << endl;
		cout << "rights." << endl;
		cout << "" << endl;
		cout << "  We protect your rights with two steps: (1) copyright the software, and" << endl;
		cout << "(2) offer you this license which gives you legal permission to copy," << endl;
		cout << "distribute and/or modify the software." << endl;
		cout << "" << endl;
		cout << "  Also, for each author's protection and ours, we want to make certain" << endl;
		cout << "that everyone understands that there is no warranty for this free" << endl;
		cout << "software.  If the software is modified by someone else and passed on, we" << endl;
		cout << "want its recipients to know that what they have is not the original, so" << endl;
		cout << "that any problems introduced by others will not reflect on the original" << endl;
		cout << "authors' reputations." << endl;
		cout << "" << endl;
		cout << "  Finally, any free program is threatened constantly by software" << endl;
		cout << "patents.  We wish to avoid the danger that redistributors of a free" << endl;
		cout << "program will individually obtain patent licenses, in effect making the" << endl;
		cout << "program proprietary.  To prevent this, we have made it clear that any" << endl;
		cout << "patent must be licensed for everyone's free use or not licensed at all." << endl;
		cout << "" << endl;
		cout << "  The precise terms and conditions for copying, distribution and" << endl;
		cout << "modification follow." << endl;
		cout << "" << endl;
		cout << "                    GNU GENERAL PUBLIC LICENSE" << endl;
		cout << "   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION" << endl;
		cout << "" << endl;
		cout << "  0. This License applies to any program or other work which contains" << endl;
		cout << "a notice placed by the copyright holder saying it may be distributed" << endl;
		cout << "under the terms of this General Public License.  The \"Program\", below," << endl;
		cout << "refers to any such program or work, and a \"work based on the Program\"" << endl;
		cout << "means either the Program or any derivative work under copyright law:" << endl;
		cout << "that is to say, a work containing the Program or a portion of it," << endl;
		cout << "either verbatim or with modifications and/or translated into another" << endl;
		cout << "language.  (Hereinafter, translation is included without limitation in" << endl;
		cout << "the term \"modification\".)  Each licensee is addressed as \"you\"." << endl;
		cout << "" << endl;
		cout << "Activities other than copying, distribution and modification are not" << endl;
		cout << "covered by this License; they are outside its scope.  The act of" << endl;
		cout << "running the Program is not restricted, and the output from the Program" << endl;
		cout << "is covered only if its contents constitute a work based on the" << endl;
		cout << "Program (independent of having been made by running the Program)." << endl;
		cout << "Whether that is true depends on what the Program does." << endl;
		cout << "" << endl;
		cout << "  1. You may copy and distribute verbatim copies of the Program's" << endl;
		cout << "source code as you receive it, in any medium, provided that you" << endl;
		cout << "conspicuously and appropriately publish on each copy an appropriate" << endl;
		cout << "copyright notice and disclaimer of warranty; keep intact all the" << endl;
		cout << "notices that refer to this License and to the absence of any warranty;" << endl;
		cout << "and give any other recipients of the Program a copy of this License" << endl;
		cout << "along with the Program." << endl;
		cout << "" << endl;
		cout << "You may charge a fee for the physical act of transferring a copy, and" << endl;
		cout << "you may at your option offer warranty protection in exchange for a fee." << endl;
		cout << "" << endl;
		cout << "  2. You may modify your copy or copies of the Program or any portion" << endl;
		cout << "of it, thus forming a work based on the Program, and copy and" << endl;
		cout << "distribute such modifications or work under the terms of Section 1" << endl;
		cout << "above, provided that you also meet all of these conditions:" << endl;
		cout << "" << endl;
		cout << "    a) You must cause the modified files to carry prominent notices" << endl;
		cout << "    stating that you changed the files and the date of any change." << endl;
		cout << "" << endl;
		cout << "    b) You must cause any work that you distribute or publish, that in" << endl;
		cout << "    whole or in part contains or is derived from the Program or any" << endl;
		cout << "    part thereof, to be licensed as a whole at no charge to all third" << endl;
		cout << "    parties under the terms of this License." << endl;
		cout << "" << endl;
		cout << "    c) If the modified program normally reads commands interactively" << endl;
		cout << "    when run, you must cause it, when started running for such" << endl;
		cout << "    interactive use in the most ordinary way, to print or display an" << endl;
		cout << "    announcement including an appropriate copyright notice and a" << endl;
		cout << "    notice that there is no warranty (or else, saying that you provide" << endl;
		cout << "    a warranty) and that users may redistribute the program under" << endl;
		cout << "    these conditions, and telling the user how to view a copy of this" << endl;
		cout << "    License.  (Exception: if the Program itself is interactive but" << endl;
		cout << "    does not normally print such an announcement, your work based on" << endl;
		cout << "    the Program is not required to print an announcement.)" << endl;
		cout << "" << endl;
		cout << "These requirements apply to the modified work as a whole.  If" << endl;
		cout << "identifiable sections of that work are not derived from the Program," << endl;
		cout << "and can be reasonably considered independent and separate works in" << endl;
		cout << "themselves, then this License, and its terms, do not apply to those" << endl;
		cout << "sections when you distribute them as separate works.  But when you" << endl;
		cout << "distribute the same sections as part of a whole which is a work based" << endl;
		cout << "on the Program, the distribution of the whole must be on the terms of" << endl;
		cout << "this License, whose permissions for other licensees extend to the" << endl;
		cout << "entire whole, and thus to each and every part regardless of who wrote it." << endl;
		cout << "" << endl;
		cout << "Thus, it is not the intent of this section to claim rights or contest" << endl;
		cout << "your rights to work written entirely by you; rather, the intent is to" << endl;
		cout << "exercise the right to control the distribution of derivative or" << endl;
		cout << "collective works based on the Program." << endl;
		cout << "" << endl;
		cout << "In addition, mere aggregation of another work not based on the Program" << endl;
		cout << "with the Program (or with a work based on the Program) on a volume of" << endl;
		cout << "a storage or distribution medium does not bring the other work under" << endl;
		cout << "the scope of this License." << endl;
		cout << "" << endl;
		cout << "  3. You may copy and distribute the Program (or a work based on it," << endl;
		cout << "under Section 2) in object code or executable form under the terms of" << endl;
		cout << "Sections 1 and 2 above provided that you also do one of the following:" << endl;
		cout << "" << endl;
 		cout << "   a) Accompany it with the complete corresponding machine-readable" << endl;
		cout << "    source code, which must be distributed under the terms of Sections" << endl;
		cout << "    1 and 2 above on a medium customarily used for software interchange; or," << endl;
		cout << "" << endl;
		cout << "    b) Accompany it with a written offer, valid for at least three" << endl;
		cout << "    years, to give any third party, for a charge no more than your" << endl;
		cout << "    cost of physically performing source distribution, a complete" << endl;
		cout << "    machine-readable copy of the corresponding source code, to be" << endl;
		cout << "    distributed under the terms of Sections 1 and 2 above on a medium" << endl;
		cout << "    customarily used for software interchange; or," << endl;
		cout << "" << endl;
		cout << "    c) Accompany it with the information you received as to the offer" << endl;
		cout << "    to distribute corresponding source code.  (This alternative is" << endl;
		cout << "    allowed only for noncommercial distribution and only if you" << endl;
		cout << "    received the program in object code or executable form with such" << endl;
		cout << "    an offer, in accord with Subsection b above.)" << endl;
		cout << "" << endl;
		cout << "The source code for a work means the preferred form of the work for" << endl;
		cout << "making modifications to it.  For an executable work, complete source" << endl;
		cout << "code means all the source code for all modules it contains, plus any" << endl;
		cout << "associated interface definition files, plus the scripts used to" << endl;
		cout << "control compilation and installation of the executable.  However, as a" << endl;
		cout << "special exception, the source code distributed need not include" << endl;
		cout << "anything that is normally distributed (in either source or binary" << endl;
		cout << "form) with the major components (compiler, kernel, and so on) of the" << endl;
		cout << "operating system on which the executable runs, unless that component" << endl;
		cout << "itself accompanies the executable." << endl;
		cout << "" << endl;
		cout << "If distribution of executable or object code is made by offering" << endl;
		cout << "access to copy from a designated place, then offering equivalent" << endl;
		cout << "access to copy the source code from the same place counts as" << endl;
		cout << "distribution of the source code, even though third parties are not" << endl;
		cout << "compelled to copy the source along with the object code." << endl;
		cout << "" << endl;
		cout << "  4. You may not copy, modify, sublicense, or distribute the Program" << endl;
		cout << "except as expressly provided under this License.  Any attempt" << endl;
		cout << "otherwise to copy, modify, sublicense or distribute the Program is" << endl;
		cout << "void, and will automatically terminate your rights under this License." << endl;
		cout << "However, parties who have received copies, or rights, from you under" << endl;
		cout << "this License will not have their licenses terminated so long as such" << endl;
		cout << "parties remain in full compliance." << endl;
		cout << "" << endl;
		cout << "  5. You are not required to accept this License, since you have not" << endl;
		cout << "signed it.  However, nothing else grants you permission to modify or" << endl;
		cout << "distribute the Program or its derivative works.  These actions are" << endl;
		cout << "prohibited by law if you do not accept this License.  Therefore, by" << endl;
		cout << "modifying or distributing the Program (or any work based on the" << endl;
		cout << "Program), you indicate your acceptance of this License to do so, and" << endl;
		cout << "all its terms and conditions for copying, distributing or modifying" << endl;
		cout << "the Program or works based on it." << endl;
		cout << "" << endl;
		cout << "  6. Each time you redistribute the Program (or any work based on the" << endl;
		cout << "Program), the recipient automatically receives a license from the" << endl;
		cout << "original licensor to copy, distribute or modify the Program subject to" << endl;
		cout << "these terms and conditions.  You may not impose any further" << endl;
		cout << "restrictions on the recipients' exercise of the rights granted herein." << endl;
		cout << "You are not responsible for enforcing compliance by third parties to" << endl;
		cout << "this License." << endl;
		cout << "" << endl;
		cout << "  7. If, as a consequence of a court judgment or allegation of patent" << endl;
		cout << "infringement or for any other reason (not limited to patent issues)," << endl;
		cout << "conditions are imposed on you (whether by court order, agreement or" << endl;
		cout << "otherwise) that contradict the conditions of this License, they do not" << endl;
		cout << "excuse you from the conditions of this License.  If you cannot" << endl;
		cout << "distribute so as to satisfy simultaneously your obligations under this" << endl;
		cout << "License and any other pertinent obligations, then as a consequence you" << endl;
		cout << "may not distribute the Program at all.  For example, if a patent" << endl;
		cout << "license would not permit royalty-free redistribution of the Program by" << endl;
		cout << "all those who receive copies directly or indirectly through you, then" << endl;
		cout << "the only way you could satisfy both it and this License would be to" << endl;
		cout << "refrain entirely from distribution of the Program." << endl;
		cout << "" << endl;
		cout << "If any portion of this section is held invalid or unenforceable under" << endl;
		cout << "any particular circumstance, the balance of the section is intended to" << endl;
		cout << "apply and the section as a whole is intended to apply in other" << endl;
		cout << "circumstances." << endl;
		cout << "" << endl;
		cout << "It is not the purpose of this section to induce you to infringe any" << endl;
		cout << "patents or other property right claims or to contest validity of any" << endl;
		cout << "such claims; this section has the sole purpose of protecting the" << endl;
		cout << "integrity of the free software distribution system, which is" << endl;
		cout << "implemented by public license practices.  Many people have made" << endl;
		cout << "generous contributions to the wide range of software distributed" << endl;
		cout << "through that system in reliance on consistent application of that" << endl;
		cout << "system; it is up to the author/donor to decide if he or she is willing" << endl;
		cout << "to distribute software through any other system and a licensee cannot" << endl;
		cout << "impose that choice." << endl;
		cout << "" << endl;
		cout << "This section is intended to make thoroughly clear what is believed to" << endl;
		cout << "be a consequence of the rest of this License." << endl;
		cout << "" << endl;
		cout << "  8. If the distribution and/or use of the Program is restricted in" << endl;
		cout << "certain countries either by patents or by copyrighted interfaces, the" << endl;
		cout << "original copyright holder who places the Program under this License" << endl;
		cout << "may add an explicit geographical distribution limitation excluding" << endl;
		cout << "those countries, so that distribution is permitted only in or among" << endl;
		cout << "countries not thus excluded.  In such case, this License incorporates" << endl;
		cout << "the limitation as if written in the body of this License." << endl;
		cout << "" << endl;
		cout << "  9. The Free Software Foundation may publish revised and/or new versions" << endl;
		cout << "of the General Public License from time to time.  Such new versions will" << endl;
		cout << "be similar in spirit to the present version, but may differ in detail to" << endl;
		cout << "address new problems or concerns." << endl;
		cout << "" << endl;
		cout << "Each version is given a distinguishing version number.  If the Program" << endl;
		cout << "specifies a version number of this License which applies to it and \"any" << endl;
		cout << "later version\", you have the option of following the terms and conditions" << endl;
		cout << "either of that version or of any later version published by the Free" << endl;
		cout << "Software Foundation.  If the Program does not specify a version number of" << endl;
		cout << "this License, you may choose any version ever published by the Free Software" << endl;
		cout << "Foundation." << endl;
		cout << "" << endl;
		cout << "  10. If you wish to incorporate parts of the Program into other free" << endl;
		cout << "programs whose distribution conditions are different, write to the author" << endl;
		cout << "to ask for permission.  For software which is copyrighted by the Free" << endl;
		cout << "Software Foundation, write to the Free Software Foundation; we sometimes" << endl;
		cout << "make exceptions for this.  Our decision will be guided by the two goals" << endl;
		cout << "of preserving the free status of all derivatives of our free software and" << endl;
		cout << "of promoting the sharing and reuse of software generally." << endl;
		cout << "" << endl;
		cout << "                            NO WARRANTY" << endl;
		cout << "" << endl;
		cout << "  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY" << endl;
		cout << "FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN" << endl;
		cout << "OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES" << endl;
		cout << "PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED" << endl;
		cout << "OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF" << endl;
		cout << "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS" << endl;
		cout << "TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE" << endl;
		cout << "PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING," << endl;
		cout << "REPAIR OR CORRECTION." << endl;
		cout << "" << endl;
		cout << "  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING" << endl;
		cout << "WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR" << endl;
		cout << "REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES," << endl;
		cout << "INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING" << endl;
		cout << "OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED" << endl;
		cout << "TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY" << endl;
		cout << "YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER" << endl;
		cout << "PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE" << endl;
		cout << "POSSIBILITY OF SUCH DAMAGES." << endl;
		cout << "" << endl;
		cout << "                     END OF TERMS AND CONDITIONS" << endl;
		return 0;
	}

	/* Mostramos la version del programa si se pide */
	if (argc == 2 &&
			(strcmp (argv[1], "--version") == 0 ||
			 strcmp (argv[1], "-v") == 0 ||
			 strcmp (argv[1], "/version") == 0))
	{
		cout << "Topologia Digital version 0.1 Copyright (C) 2006 Javier Carlos Jeronimo Suarez" << endl << endl;
		return 0;
	}

	/* Comprobamos el numero de argumentos recibidos */
	if (argc < 3)
	{
		cout << "Topologia Digital version 1 Copyright (C) 2006 Javier Carlos Jeronimo Suarez" << endl << endl;
		cout << "Topologia Digital comes with ABSOLUTELY NO WARRANTY. This is free software," << endl;
		cout << "and you are welcome to redistribute it under certain conditions." << endl;
		cout << "Type `topologia_digital /license' for details." << endl << endl;
		cout << "Author: Javier Carlos Jeronimo Suarez <jcjeronimo@alumnos.upm.es>" << endl;
		cout << "        Digital Topology Course" << endl;
		cout << "        2005-2006 Year" << endl;
		cout << "        Applied Mathematics Department" << endl;
		cout << "        Computer Science Faculty" << endl;
		cout << "        Polytechnic University of Madrid" << endl << endl;
		cout << "Use: topologia_digital <operations_file> <image.bmp> [DEBUG]" << endl << endl;
		cout << "See operations.txt for more information." << endl << endl;
		return 0;
	}

	operations_filename.assign (argv[1]);

	image_filename.assign (argv[2]);

    if (argc > 3 && (strcmp (argv[3], "DEBUG") == 0))
	    OP_image = new operations(image_filename, operations_filename, true);
    else
        OP_image = new operations(image_filename, operations_filename, false);

	// Destruye el objeto
	delete OP_image;

	// Finalizamos la ejecucion
	return 0;
}
