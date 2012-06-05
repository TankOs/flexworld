#pragma once

#include <Rocket/Controls/DataSource.h>
#include <string>

class Controls;

/** libRocket bindings data source.
 */
class BindingDataSource : public Rocket::Controls::DataSource {
	public:
		/** Ctor.
		 * @param controls Controls.
		 */
		BindingDataSource( const Controls& controls );

		/** Get number of rows.
		 * @param table Table name.
		 * @return Number of rows.
		 */
		int GetNumRows( const Rocket::Core::String& table );

		/** Get row.
		 * @param row Row.
		 * @param table Table name.
		 * @param row_index Row index.
		 * @param columns Columns.
		 */
		void GetRow( Rocket::Core::StringList& row, const Rocket::Core::String& table, int row_index, const Rocket::Core::StringList& columns );

	private:
		const Controls& m_controls;
};
