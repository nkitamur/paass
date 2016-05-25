/** \file Unpacker.hpp
 * \brief A class to handle the unpacking of UTK/ORNL style pixie16 data spills.
 *
 * This class is intended to be used as a replacement of pixiestd.cpp from Stan
 * Paulauskas's pixie_scan. The majority of function names and arguments are
 * preserved as much as possible while allowing for more standardized unpacking
 * of pixie16 data.
 * CRT
 *
 * \author C. R. Thornsberry
 * \date Feb. 12th, 2016
 */
#ifndef UNPACKER_HPP
#define UNPACKER_HPP

#include <deque>
#include <vector>
#include <string>

#ifndef MAX_PIXIE_MOD
#define MAX_PIXIE_MOD 12
#endif
#ifndef MAX_PIXIE_CHAN
#define MAX_PIXIE_CHAN 15
#endif

class XiaEvent;
class ScanMain;

class Unpacker{
  private:
	unsigned int TOTALREAD; /// Maximum number of data words to read.
	unsigned int maxWords; /// Maximum number of data words for revision D.
	
	unsigned int channel_counts[MAX_PIXIE_MOD+1][MAX_PIXIE_CHAN+1]; /// Counters for each channel in each module.

	/** Scan the event list and sort it by timestamp.
	  * \return Nothing.
	  */
	void TimeSort();

	/** Scan the time sorted event list and package the events into a raw
	  * event with a size governed by the event width.
	  * \return True if the rawEvent is not empty and false otherwise.
	  */
	bool BuildRawEvent();
	
	/** Push an event into the event list.
	  * \param[in]  event_ The XiaEvent to push onto the back of the event list.
	  * \return True if the XiaEvent's module number is valid and false otherwise.
	  */
	bool AddEvent(XiaEvent *event_);
	
	/** Clear all events in the spill event list. WARNING! This method will delete all events in the
	  * event list. This could cause seg faults if the events are used elsewhere.
	  * \return Nothing.
	  */	
	void ClearEventList();

	/** Clear all events in the raw event list. WARNING! This method will delete all events in the
	  * event list. This could cause seg faults if the events are used elsewhere.
	  * \return Nothing.
	  */	
	void ClearRawEvent();

  protected:
	unsigned int eventWidth; /// The width of the raw event in pixie clock ticks (8 ns).
	
	bool debug_mode; /// True if debug mode is set.

	std::vector<std::deque<XiaEvent*> > eventList; /// The list of all events in a spill.
	std::deque<XiaEvent*> rawEvent; /// The list of all events in the event window.

	/** Process all events in the event list.
	  * \param[in]  addr_ Pointer to a location in memory. Unused by default.
	  * \return Nothing.
	  */
	virtual void ProcessRawEvent(void *addr_=NULL);
	
	/** Add an event to generic statistics output.
	  * \param[in]  event_ Pointer to the current XIA event. Unused by default.
	  * \return Nothing.
	  */
	virtual void RawStats(XiaEvent *event_){  }
	
	/** Called form ReadSpill. Scan the current spill and construct a list of
	  * events which fired by obtaining the module, channel, trace, etc. of the
	  * timestamped event. This method will construct the event list for
	  * later processing.
	  * \param[in]  buf    Pointer to an array of unsigned ints containing raw buffer data.
	  * \param[out] bufLen The number of words in the buffer.
	  * \return The number of XiaEvents read from the buffer.
	  */	
	int ReadBuffer(unsigned int *buf, unsigned long &bufLen);
	
  public:
  	/// Default constructor.
	Unpacker();

	/// Destructor.
	virtual ~Unpacker();

	/// Return the maximum module read from the input file.
	size_t GetMaxModule(){ return eventList.size(); }

	/// Return the width of the raw event window in pixie16 clock ticks.
	unsigned int GetEventWidth(){ return eventWidth; }

	/// Toggle debug mode on / off.
	bool SetDebugMode(bool state_=true){ return (debug_mode = state_); }
	
	/// Set the width of events in pixie16 clock ticks.
	unsigned int SetEventWidth(unsigned int width_){ return (eventWidth = width_); }
	
	/** ReadSpill is responsible for constructing a list of pixie16 events from
	  * a raw data spill. This method performs sanity checks on the spill and
	  * calls ReadBuffer in order to construct the event list.
	  * \param[in]  data       Pointer to an array of unsigned ints containing the spill data.
	  * \param[in]  nWords     The number of words in the array.
	  * \param[in]  is_verbose Toggle the verbosity flag on/off.
	  * \return True if the spill was read successfully and false otherwise.
	  */	
	bool ReadSpill(unsigned int *data, unsigned int nWords, bool is_verbose=true);
	
	/** Empty the raw event and the event list.
	  * \param[in]  write_count_file Toggle writting of raw channel counts to file.
	  * \return Nothing.
	  */
	void Close(bool write_count_file=false);
};

#endif
