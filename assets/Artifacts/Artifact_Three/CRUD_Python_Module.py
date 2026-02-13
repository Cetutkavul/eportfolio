# CRUD_Python_Module.py
# Author: Misty Tutkavul
# Course: CS-340 (Client/Server Development)
# Milestone: Modules 4–5 - Implement full CRUD operations for MongoDB
#
# Description:
#   This module provides a reusable, object-oriented CRUD (Create, Read, Update, Delete)
#   interface for interacting with a MongoDB collection. It is designed to support
#   database-backed applications such as Dash dashboards while enforcing defensive
#   programming, secure access patterns, and maintainable database logic.

# Key Design Principles:
# Separation of concerns: Database logic is isolated from UI and controller layers
# Defensive programming: Input validation and exception handling throughout
# Security-aware: Supports MongoDB role-based access control (RBAC)
# Scalability-ready: Designed to work with indexing and schema validation


from typing import Any, Dict, List, Optional, Sequence, Tuple
from urllib.parse import quote_plus
from pymongo import MongoClient
from pymongo.collection import Collection
from pymongo.errors import PyMongoError, ConnectionFailure, OperationFailure


class AnimalShelter:
    """
    Provides CRUD operations for the 'animals' collection
    in the 'aac' MongoDB database.

    This class acts as the Model layer in an MVC-style architecture
    and is intended to be imported by controller or dashboard code.
    """

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
        Initialize the MongoDB client connection and verify credentials.

        Args:
            user: MongoDB username (RBAC enforced server-side)
            password: MongoDB password
            host: Database host
            port: MongoDB port
            auth_db: Authentication database
            db_name: Target database name
            collection_name: Target collection
        """

        # URL-encode password to safely include special characters
        safe_pwd = quote_plus(password)

        # Build authenticated MongoDB connection URI
        uri = f"mongodb://{user}:{safe_pwd}@{host}:{port}/?authSource={auth_db}"

        # Ensure connection fails quickly if MongoDB is unreachable
        default_kwargs = {"serverSelectionTimeoutMS": 5000}
        default_kwargs.update(kwargs)

        try:
            self.client = MongoClient(uri, **default_kwargs)
            # Explicit ping to validate credentials and connectivity
            self.client.admin.command("ping")
        except (ConnectionFailure, OperationFailure) as e:
            raise RuntimeError(f"MongoDB connection failed: {e}") from e

        # Assign database and collection references
        self.database = self.client[db_name]
        self.collection: Collection = self.database[collection_name]

    # -----------------------------------------------------------------
    # CREATE
    # -----------------------------------------------------------------
    def create(self, data: Dict[str, Any]) -> bool:
        """
        Insert a single document into the collection.

        Args:
            data: Dictionary representing a MongoDB document

        Returns:
            True if insert succeeds, False otherwise
        """
        if not isinstance(data, dict) or not data:
            return False

        try:
            result = self.collection.insert_one(data)
            return bool(result.acknowledged)
        except PyMongoError as e:
            print(f"Insert error: {e}")
            return False

    # -----------------------------------------------------------------
    # READ (Enhanced with defensive validation)
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
        Retrieve documents matching a query.

        Defensive features:
        - Ensures query and projection are dictionaries
        - Prevents malformed input from crashing the application
        - Supports sorting, pagination, and field projection

        Args:
            query: MongoDB filter dictionary
            projection: Fields to include/exclude
            limit: Max number of documents
            sort: Sort specification
            skip: Documents to skip (pagination)

        Returns:
            List of MongoDB documents
        """
        try:
            query = query if isinstance(query, dict) else {}
            projection = projection if isinstance(projection, dict) else None

            cursor = self.collection.find(query, projection)

            if sort:
                cursor = cursor.sort(list(sort))
            if skip > 0:
                cursor = cursor.skip(skip)
            if limit > 0:
                cursor = cursor.limit(limit)

            return list(cursor)
        except PyMongoError as e:
            print(f"Read error: {e}")
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
        Update document(s) matching a filter.

        Automatically wraps plain update dictionaries
        in a MongoDB $set operator if needed.

        Args:
            filter: Query filter
            update: Update document or field dictionary
            many: Whether to update multiple documents

        Returns:
            Number of modified documents
        """
        if not isinstance(filter, dict):
            raise TypeError("Filter must be a dictionary")
        if not isinstance(update, dict) or not update:
            raise TypeError("Update must be a non-empty dictionary")

        update_doc = update if any(k.startswith("$") for k in update) else {"$set": update}

        try:
            result = (
                self.collection.update_many(filter, update_doc)
                if many
                else self.collection.update_one(filter, update_doc)
            )
            return int(result.modified_count or 0)
        except PyMongoError as e:
            print(f"Update error: {e}")
            return 0

    # -----------------------------------------------------------------
    # DELETE
    # -----------------------------------------------------------------
    def delete(self, filter: Dict[str, Any], many: bool = False) -> int:
        """
        Delete document(s) matching a filter.

        Args:
            filter: Query dictionary
            many: Whether to delete multiple documents

        Returns:
            Number of deleted documents
        """
        if not isinstance(filter, dict):
            raise TypeError("Filter must be a dictionary")

        try:
            result = (
                self.collection.delete_many(filter)
                if many
                else self.collection.delete_one(filter)
            )
            return int(result.deleted_count or 0)
        except PyMongoError as e:
            print(f"Delete error: {e}")
            return 0

    def close(self) -> None:
        """Safely close the MongoDB client connection."""
        try:
            self.client.close()
        except Exception:
            pass