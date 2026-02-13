# CRUD_Python_Module.py
# Author: Misty Tutkavul
# Course: CS-340 (Client/Server Development)
# Milestone: Modules 4–5 - Implement full CRUD operations for MongoDB
#
# Purpose:
#   This module provides an object-oriented (class-based) interface
#   for performing Create, Read, Update, and Delete (CRUD) operations on
#   the 'animals' collection of the 'aac' MongoDB database.
#   Other scripts (e.g., dashboards or test notebooks) can import
#   and reuse this class to interact with the same database.

# ---------------------------------------------------------------------
# Library Imports
# ---------------------------------------------------------------------
from typing import Any, Dict, List, Optional, Sequence, Tuple
from urllib.parse import quote_plus
from pymongo import MongoClient
from pymongo.collection import Collection
from pymongo.errors import PyMongoError, ConnectionFailure, OperationFailure


# ---------------------------------------------------------------------
# CRUD Class Definition
# ---------------------------------------------------------------------
class AnimalShelter(object):
    """CRUD operations for the 'animals' collection in MongoDB."""

    # -----------------------------------------------------------------
    # Constructor (Initialization)
    # -----------------------------------------------------------------
    def __init__(
        self,
        user: str = "aacuser",
        password: str = "s3CuR3P@ssw0rd!",
        host: str = "localhost",
        port: int = 27017,
        auth_db: str = "admin",
        db_name: str = "aac",
        collection_name: str = "animals",
        **kwargs: Any,
    ):
        """
        Initializes a MongoDB client connection and sets the active
        database and collection references. Uses authentication with
        the aacuser account.

        Example:
            shelter = AnimalShelter(user="aacuser", password="yourpass")
        """
        # Encode password for URI safety
        safe_pwd = quote_plus(password)

        # Build connection URI (authenticate against admin DB)
        uri = f"mongodb://{user}:{safe_pwd}@{host}:{port}/?authSource={auth_db}"

        # Keep the client responsive if server is unreachable
        default_kwargs = {"serverSelectionTimeoutMS": 5000}
        default_kwargs.update(kwargs)

        try:
            self.client = MongoClient(uri, **default_kwargs)
            # Validate connectivity and credentials
            self.client.admin.command("ping")
        except (ConnectionFailure, OperationFailure) as e:
            raise RuntimeError(f"MongoDB connection or authentication failed: {e}") from e

        # Select database and collection
        self.database = self.client[db_name]
        self.collection: Collection = self.database[collection_name]

    # -----------------------------------------------------------------
    # CREATE
    # -----------------------------------------------------------------
    def create(self, data: Dict[str, Any]) -> bool:
        """
        Insert a single document.

        Args:
            data: MongoDB document as a dict (e.g., {"name": "Bella", "animal_type": "Dog"}).

        Returns:
            True if the insert was acknowledged; otherwise False.
        """
        if not isinstance(data, dict) or not data:
            return False
        try:
            result = self.collection.insert_one(data)
            return bool(result.acknowledged)
        except PyMongoError as e:
            print(f"Error during insert: {e}")
            return False

    # -----------------------------------------------------------------
    # READ
    # -----------------------------------------------------------------
    def read(
        self,
        query: Optional[Dict[str, Any]] = None,
        projection: Optional[Dict[str, int]] = None,
        limit: int = 0,
        sort: Optional[Sequence[Tuple[str, int]]] = None,
        skip: int = 0,
    ) -> List[Dict[str, Any]]:
        """
        Query documents using find() and return them as a list.

        Args:
            query: Filter dict (default: {}).
            projection: Fields to include/exclude (e.g., {"_id": 0, "name": 1}).
            limit: Max number of documents (0 = no limit).
            sort: List of (field, direction) tuples, e.g., [("age_upon_outcome_in_weeks", 1)].
            skip: Number of docs to skip.

        Returns:
            List of documents; empty list on error.
        """
        try:
            query = query or {}
            cursor = self.collection.find(query, projection)
            if sort:
                cursor = cursor.sort(list(sort))
            if skip > 0:
                cursor = cursor.skip(skip)
            if limit > 0:
                cursor = cursor.limit(limit)
            return list(cursor)
        except PyMongoError as e:
            print(f"Error during read: {e}")
            return []

    # -----------------------------------------------------------------
    # UPDATE
    # -----------------------------------------------------------------
    def update(
        self,
        filter: Dict[str, Any],
        update: Dict[str, Any],
        many: bool = False,
    ) -> int:
        """
        Update document(s) that match the filter.

        Args:
            filter: Query dict to match documents (e.g., {"breed": "Pit Bull Mix"}).
            update: Either a proper MongoDB update document (e.g., {"$set": {...}})
                    OR a plain dict of fields to set (e.g., {"name": "Luna"}), which
                    will be auto-wrapped into {"$set": ...}.
            many:   If True, use update_many; otherwise update_one.

        Returns:
            The number of documents modified (int).
        """
        if not isinstance(filter, dict):
            raise TypeError("filter must be a dict")
        if not isinstance(update, dict) or not update:
            raise TypeError("update must be a non-empty dict")

        # If caller passed a plain field dict (no operators), wrap in $set
        has_operator = any(k.startswith("$") for k in update.keys())
        update_doc = update if has_operator else {"$set": update}

        try:
            if many:
                result = self.collection.update_many(filter, update_doc)
            else:
                result = self.collection.update_one(filter, update_doc)
            return int(result.modified_count or 0)
        except PyMongoError as e:
            print(f"Error during update: {e}")
            return 0

    # -----------------------------------------------------------------
    # DELETE
    # -----------------------------------------------------------------
    def delete(
        self,
        filter: Dict[str, Any],
        many: bool = False,
    ) -> int:
        """
        Delete document(s) that match the filter.

        Args:
            filter: Query dict to match documents for deletion.
            many:   If True, use delete_many; otherwise delete_one.

        Returns:
            The number of documents deleted (int).
        """
        if not isinstance(filter, dict):
            raise TypeError("filter must be a dict")

        try:
            if many:
                result = self.collection.delete_many(filter)
            else:
                result = self.collection.delete_one(filter)
            return int(result.deleted_count or 0)
        except PyMongoError as e:
            print(f"Error during delete: {e}")
            return 0

    # -----------------------------------------------------------------
    # Close Connection
    # -----------------------------------------------------------------
    def close(self) -> None:
        """Close the MongoDB client connection."""
        try:
            self.client.close()
        except Exception:
            pass
